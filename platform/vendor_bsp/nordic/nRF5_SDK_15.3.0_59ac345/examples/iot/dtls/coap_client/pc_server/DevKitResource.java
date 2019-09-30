package org.eclipse.californium.examples;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.server.resources.CoapExchange;

public class DevKitResource extends CoapResource {
	
	private String currentState;

    public DevKitResource(String resourceName) {
        
        // set resource identifier
        super(resourceName);
        
        // set display name
        getAttributes().setTitle("nRF DK Resource");
    }
    
    @Override
    public void handleGET(CoapExchange exchange) {
        
        // respond to the request
        exchange.respond(currentState);
    }

    @Override
    public void handlePUT(CoapExchange exchange) {
        
        byte[] payload = exchange.getRequestPayload();

        try {
        	currentState = new String(payload, "UTF-8");
        	exchange.respond(ResponseCode.CHANGED);
        	System.out.println("");
        	System.out.println("**************************************************");
        	System.out.println("\"" + this.getName() + "\": PUT request received with value: " + currentState);
        	System.out.println("**************************************************");
        	System.out.println("");
    	} catch (Exception e) {
    		exchange.respond(ResponseCode.NOT_ACCEPTABLE);
        }
    }
}
