/*******************************************************************************
 * Copyright (c) 2014 Institute for Pervasive Computing, ETH Zurich and others.
 * 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * 
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.html.
 * 
 * Contributors:
 *    Matthias Kovatsch - creator and main architect
 ******************************************************************************/
package org.eclipse.californium.tools;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.URI;
import java.net.URISyntaxException;
import java.security.GeneralSecurityException;
import java.security.KeyStore;
import java.security.PrivateKey;
import java.security.cert.Certificate;
import java.util.logging.Level;

import org.eclipse.californium.core.CaliforniumLogger;
import org.eclipse.californium.core.Utils;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.core.coap.Request;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.network.CoAPEndpoint;
import org.eclipse.californium.core.network.Endpoint;
import org.eclipse.californium.core.network.EndpointManager;
import org.eclipse.californium.core.network.config.NetworkConfig;
import org.eclipse.californium.elements.RawData;
import org.eclipse.californium.elements.RawDataChannel;
import org.eclipse.californium.scandium.DTLSConnector;
import org.eclipse.californium.scandium.ScandiumLogger;
import org.eclipse.californium.scandium.config.DtlsConnectorConfig;
import org.eclipse.californium.scandium.dtls.cipher.CipherSuite;
import org.eclipse.californium.scandium.dtls.pskstore.InMemoryPskStore;
import org.eclipse.californium.scandium.dtls.pskstore.StaticPskStore;

/**
 * This class implements a simple CoAP client for testing purposes. Usage:
 * <p>
 * {@code java -jar SampleClient.jar [-l] METHOD URI [PAYLOAD]}
 * <ul>
 * <li>METHOD: {GET, POST, PUT, DELETE, DISCOVER, OBSERVE}
 * <li>URI: The URI to the remote endpoint or resource}
 * <li>PAYLOAD: The data to send with the request}
 * </ul>
 * Options:
 * <ul>
 * <li>-l: Loop for multiple responses}
 * </ul>
 * Examples:
 * <ul>
 * <li>{@code SampleClient DISCOVER coap://localhost}
 * <li>{@code SampleClient POST coap://someServer.org:5683 my data}
 * </ul>
 */
public class Nrf5xConsoleClient {

	static {
		CaliforniumLogger.initialize();
		CaliforniumLogger.setLevel(Level.WARNING);
		
		ScandiumLogger.initialize();
		ScandiumLogger.setLevel(Level.FINER);
	}
	
	// the trust store file used for DTLS server authentication
    private static final String TRUST_STORE_LOCATION = "certs/trustStore.jks";
	private static final String TRUST_STORE_PASSWORD = "rootPass";

	private static final String KEY_STORE_LOCATION = "certs/keyStore.jks";
	private static final String KEY_STORE_PASSWORD = "endPass";
	
	// resource URI path used for discovery
	private static final String DISCOVERY_RESOURCE = "/.well-known/core";

	// indices of command line parameters
	private static final int IDX_METHOD          = 0;
	private static final int IDX_URI             = 1;
	private static final int IDX_PAYLOAD         = 2;

	// exit codes for runtime errors
	private static final int ERR_MISSING_METHOD  = 1;
	private static final int ERR_UNKNOWN_METHOD  = 2;
	private static final int ERR_MISSING_URI     = 3;
	private static final int ERR_BAD_URI         = 4;
	private static final int ERR_REQUEST_FAILED  = 5;
	private static final int ERR_RESPONSE_FAILED = 6;


	// initialize parameters
	static String method = null;
	static URI uri = null;
	static String payload = "";
	static boolean loop = false;

	static boolean usePSK = false;
	static boolean useRaw = true;

	// for coaps
	private static Endpoint dtlsEndpoint;
	private static DTLSConnector dtlsConnector;
	
	/*
	 * Main method of this client.
	 */
	public static void main(String[] args) throws IOException, GeneralSecurityException {
		
		// display help if no parameters specified
		if (args.length == 0) {
			printInfo();
			return;
		}
		
		// input parameters
		int i_all = 0;
		int i_rep = -1;
		int n_rep = 1;
		int idx = 0;
		for (String arg : args) {
			i_all++;
			if (arg.startsWith("-")) {
				if (arg.equals("-l")) {
					loop = true;
				} else if (arg.equals("-psk")) {
					usePSK = true;
				} else if (arg.equals("-cert")) {
					useRaw = false;
				} else if (arg.equals("-rep")) {
					i_rep = i_all;
				} else {
					System.out.println("Unrecognized option: " + arg);
				}
			} else {
				if ((i_rep > 0) && (i_all == (i_rep + 1)))
				{
					n_rep = Integer.parseInt(arg);
				}
				else
				{
					switch (idx) {
						case IDX_METHOD:
							method = arg.toUpperCase();
							break;
						case IDX_URI:
							try {
								uri = new URI(arg);
							} catch (URISyntaxException e) {
								System.err.println("Failed to parse URI: " + e.getMessage());
								System.exit(ERR_BAD_URI);
							}
							break;
						case IDX_PAYLOAD:
							payload = arg;
							break;
						default:
							System.out.println("Unexpected argument: " + arg);
					}
					++idx;
				}
			}
		}

		// check if mandatory parameters specified
		if (method == null) {
			System.err.println("Method not specified");
			System.exit(ERR_MISSING_METHOD);
		}
		if (uri == null) {
			System.err.println("URI not specified");
			System.exit(ERR_MISSING_URI);
		}
		
		for (int i=0; i<n_rep; i++)
		{
			// create request according to specified method
			Request request = newRequest(method);
	
			// set request URI
			if (method.equals("DISCOVER") && (uri.getPath() == null || uri.getPath().isEmpty() || uri.getPath().equals("/"))) {
				// add discovery resource path to URI
				try {
					uri = new URI(uri.getScheme(), uri.getAuthority(), DISCOVERY_RESOURCE, uri.getQuery());
					
				} catch (URISyntaxException e) {
					System.err.println("Failed to parse URI: " + e.getMessage());
					System.exit(ERR_BAD_URI);
				}
			}
			
			request.setURI(uri);
			request.setPayload(payload);
			request.getOptions().setContentFormat(MediaTypeRegistry.TEXT_PLAIN);
			
			if (request.getScheme().equals(CoAP.COAP_SECURE_URI_SCHEME)) {
				if (i == 0) {
					try {
						// load key store
						KeyStore keyStore = KeyStore.getInstance("JKS");
						InputStream in = new FileInputStream(KEY_STORE_LOCATION);
						keyStore.load(in, KEY_STORE_PASSWORD.toCharArray());
		
						// load trust store
						KeyStore trustStore = KeyStore.getInstance("JKS");
						InputStream inTrust = new FileInputStream(TRUST_STORE_LOCATION);
						trustStore.load(inTrust, TRUST_STORE_PASSWORD.toCharArray());
		
						// You can load multiple certificates if needed
						Certificate[] trustedCertificates = new Certificate[1];
						trustedCertificates[0] = trustStore.getCertificate("root");
		
						InetAddress myIPv6Addr = Inet6Address.getByName("Your IPv6 address as string.");
						InetSocketAddress myInetSocketAddr = new InetSocketAddress(myIPv6Addr, 5694);
						DtlsConnectorConfig.Builder builder = new DtlsConnectorConfig.Builder(myInetSocketAddr);
						builder.setPskStore(new StaticPskStore("Client_identity", "secretPSK".getBytes()));
						builder.setIdentity((PrivateKey)keyStore.getKey("client", KEY_STORE_PASSWORD.toCharArray()),
								keyStore.getCertificateChain("client"), true);
						builder.setTrustStore(trustedCertificates);
						dtlsConnector = new DTLSConnector(builder.build(), null);
						dtlsConnector.setRawDataReceiver(new RawDataChannelImpl());
		
					} catch (GeneralSecurityException | IOException e) {
						System.err.println("Could not load the keystore");
						e.printStackTrace();
					}
			        
					dtlsEndpoint = new CoAPEndpoint(dtlsConnector, NetworkConfig.getStandard());
					dtlsEndpoint.start();
					EndpointManager.getEndpointManager().setDefaultSecureEndpoint(dtlsEndpoint);
				}
			}
			
			// execute request
			try {
				request.send();
	
				// loop for receiving multiple responses
				do {
		
					// receive response
					Response response = null;
					try {
						response = request.waitForResponse();
					} catch (InterruptedException e) {
						System.err.println("Failed to receive response: " + e.getMessage());
						System.exit(ERR_RESPONSE_FAILED);
					}
		
					// output response
		
					if (response != null) {
		
						System.out.println(Utils.prettyPrint(response));
						System.out.println("Time elapsed (ms): " + response.getRTT());
		
						// check of response contains resources
						if (response.getOptions().isContentFormat(MediaTypeRegistry.APPLICATION_LINK_FORMAT)) {
		
							String linkFormat = response.getPayloadString();
		
							// output discovered resources
							System.out.println("\nDiscovered resources:");
							System.out.println(linkFormat);
		
						} else {
							// check if link format was expected by client
							if (method.equals("DISCOVER")) {
								System.out.println("Server error: Link format not specified");
							}
						}
		
					} else {
						// no response received	
						System.err.println("Request timed out");
						break;
					}
		
				} while (loop);
				
			} catch (Exception e) {
				System.err.println("Failed to execute request: " + e.getMessage());
				System.exit(ERR_REQUEST_FAILED);
			}

			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	/*
	 * Outputs user guide of this program.
	 */
	public static void printInfo() {
		System.out.println("Californium (Cf) Console Client");
		System.out.println("(c) 2014, Institute for Pervasive Computing, ETH Zurich");
		System.out.println();
		System.out.println("Usage: " + Nrf5xConsoleClient.class.getSimpleName() + " [-l] METHOD URI [PAYLOAD]");
		System.out.println("  METHOD  : {GET, POST, PUT, DELETE, DISCOVER, OBSERVE}");
		System.out.println("  URI     : The CoAP URI of the remote endpoint or resource");
		System.out.println("            A coaps URI will automatically use CoAP over DTLS");
		System.out.println("  PAYLOAD : The data to send with the request");
		System.out.println("Options:");
		System.out.println("  -rep N  : Do a request N number of times with the same parameters, with 1 sec delay between requests.");
		System.out.println("  -l      : Loop for multiple responses");
		System.out.println("           (automatic for OBSERVE and separate responses)");
		System.out.println("  -psk    : Use a pre-shared secrest for DTLS (is prompted)");
		System.out.println("  -cert   : Use full X.509 certificates instead of raw public keys");
		System.out.println();
		System.out.println("Examples:");
		System.out.println("  " + Nrf5xConsoleClient.class.getSimpleName() + " DISCOVER coap://localhost");
		System.out.println("  " + Nrf5xConsoleClient.class.getSimpleName() + " PUT coap://iot.eclipse.org:5683/large-put my data");
	}

	private static class RawDataChannelImpl implements RawDataChannel {

		// @Override
		public void receiveData(final RawData raw) {

			System.out.println("Received response: " + new String(raw.getBytes()));

			dtlsConnector.destroy();
		}
	}	

	/*
	 * Instantiates a new request based on a string describing a method.
	 * 
	 * @return A new request object, or null if method not recognized
	 */
	private static Request newRequest(String method) {
		if (method.equals("GET")) {
			return Request.newGet();
		} else if (method.equals("POST")) {
			return Request.newPost();
		} else if (method.equals("PUT")) {
			return Request.newPut();
		} else if (method.equals("DELETE")) {
			return Request.newDelete();
		} else if (method.equals("DISCOVER")) {
			return Request.newGet();
		} else if (method.equals("OBSERVE")) {
			Request request = Request.newGet();
			request.setObserve();
			loop = true;
			return request;
		} else {
			System.err.println("Unknown method: " + method);
			System.exit(ERR_UNKNOWN_METHOD);
			return null;
		}
	}

}
