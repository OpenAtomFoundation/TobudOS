/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x100000
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 0x40000
}

SECTIONS
{
}

SECTIONS
{
  . = ALIGN(4);
  .mem_section_dummy_ram :
  {
  }
  .log_dynamic_data :
  {
    PROVIDE(__start_log_dynamic_data = .);
    KEEP(*(SORT(.log_dynamic_data*)))
    PROVIDE(__stop_log_dynamic_data = .);
  } > RAM
  .log_filter_data :
  {
    PROVIDE(__start_log_filter_data = .);
    KEEP(*(SORT(.log_filter_data*)))
    PROVIDE(__stop_log_filter_data = .);
  } > RAM

} INSERT AFTER .data;

SECTIONS
{
  .mem_section_dummy_rom :
  {
  }
  .crypto_data :
  {
    PROVIDE(__start_crypto_data = .);
    KEEP(*(SORT(.crypto_data*)))
    PROVIDE(__stop_crypto_data = .);
  } > FLASH
  .test_vector_aes_func_data :
  {
    PROVIDE(__start_test_vector_aes_func_data = .);
    KEEP(*(SORT(.test_vector_aes_func_data*)))
    PROVIDE(__stop_test_vector_aes_func_data = .);
  } > FLASH
  .test_vector_aes_data :
  {
    PROVIDE(__start_test_vector_aes_data = .);
    KEEP(*(SORT(.test_vector_aes_data*)))
    PROVIDE(__stop_test_vector_aes_data = .);
  } > FLASH
  .test_vector_aes_mac_data :
  {
    PROVIDE(__start_test_vector_aes_mac_data = .);
    KEEP(*(SORT(.test_vector_aes_mac_data*)))
    PROVIDE(__stop_test_vector_aes_mac_data = .);
  } > FLASH
  .test_vector_aes_monte_carlo_data :
  {
    PROVIDE(__start_test_vector_aes_monte_carlo_data = .);
    KEEP(*(SORT(.test_vector_aes_monte_carlo_data*)))
    PROVIDE(__stop_test_vector_aes_monte_carlo_data = .);
  } > FLASH
  .test_vector_aead_data :
  {
    PROVIDE(__start_test_vector_aead_data = .);
    KEEP(*(SORT(.test_vector_aead_data*)))
    PROVIDE(__stop_test_vector_aead_data = .);
  } > FLASH
  .test_vector_aead_simple_data :
  {
    PROVIDE(__start_test_vector_aead_simple_data = .);
    KEEP(*(SORT(.test_vector_aead_simple_data*)))
    PROVIDE(__stop_test_vector_aead_simple_data = .);
  } > FLASH
  .test_vector_ecdh_data_deterministic_full :
  {
    PROVIDE(__start_test_vector_ecdh_data_deterministic_full = .);
    KEEP(*(SORT(.test_vector_ecdh_data_deterministic_full*)))
    PROVIDE(__stop_test_vector_ecdh_data_deterministic_full = .);
  } > FLASH
  .test_vector_ecdh_data_random :
  {
    PROVIDE(__start_test_vector_ecdh_data_random = .);
    KEEP(*(SORT(.test_vector_ecdh_data_random*)))
    PROVIDE(__stop_test_vector_ecdh_data_random = .);
  } > FLASH
  .test_vector_ecdh_data_deterministic_simple :
  {
    PROVIDE(__start_test_vector_ecdh_data_deterministic_simple = .);
    KEEP(*(SORT(.test_vector_ecdh_data_deterministic_simple*)))
    PROVIDE(__stop_test_vector_ecdh_data_deterministic_simple = .);
  } > FLASH
  .test_vector_ecdsa_random_data :
  {
    PROVIDE(__start_test_vector_ecdsa_random_data = .);
    KEEP(*(SORT(.test_vector_ecdsa_random_data*)))
    PROVIDE(__stop_test_vector_ecdsa_random_data = .);
  } > FLASH
  .test_vector_ecdsa_verify_data :
  {
    PROVIDE(__start_test_vector_ecdsa_verify_data = .);
    KEEP(*(SORT(.test_vector_ecdsa_verify_data*)))
    PROVIDE(__stop_test_vector_ecdsa_verify_data = .);
  } > FLASH
  .test_vector_ecdsa_sign_data :
  {
    PROVIDE(__start_test_vector_ecdsa_sign_data = .);
    KEEP(*(SORT(.test_vector_ecdsa_sign_data*)))
    PROVIDE(__stop_test_vector_ecdsa_sign_data = .);
  } > FLASH
  .test_vector_eddsa_verify_data :
  {
    PROVIDE(__start_test_vector_eddsa_verify_data = .);
    KEEP(*(SORT(.test_vector_eddsa_verify_data*)))
    PROVIDE(__stop_test_vector_eddsa_verify_data = .);
  } > FLASH
  .test_vector_eddsa_sign_data :
  {
    PROVIDE(__start_test_vector_eddsa_sign_data = .);
    KEEP(*(SORT(.test_vector_eddsa_sign_data*)))
    PROVIDE(__stop_test_vector_eddsa_sign_data = .);
  } > FLASH
  .test_vector_hmac_data :
  {
    PROVIDE(__start_test_vector_hmac_data = .);
    KEEP(*(SORT(.test_vector_hmac_data*)))
    PROVIDE(__stop_test_vector_hmac_data = .);
  } > FLASH
  .test_vector_hkdf_data :
  {
    PROVIDE(__start_test_vector_hkdf_data = .);
    KEEP(*(SORT(.test_vector_hkdf_data*)))
    PROVIDE(__stop_test_vector_hkdf_data = .);
  } > FLASH
  .test_vector_hash_data :
  {
    PROVIDE(__start_test_vector_hash_data = .);
    KEEP(*(SORT(.test_vector_hash_data*)))
    PROVIDE(__stop_test_vector_hash_data = .);
  } > FLASH
  .test_vector_hash_long_data :
  {
    PROVIDE(__start_test_vector_hash_long_data = .);
    KEEP(*(SORT(.test_vector_hash_long_data*)))
    PROVIDE(__stop_test_vector_hash_long_data = .);
  } > FLASH
  .test_case_data :
  {
    PROVIDE(__start_test_case_data = .);
    KEEP(*(SORT(.test_case_data*)))
    PROVIDE(__stop_test_case_data = .);
  } > FLASH
    .nrf_queue :
  {
    PROVIDE(__start_nrf_queue = .);
    KEEP(*(.nrf_queue))
    PROVIDE(__stop_nrf_queue = .);
  } > FLASH
  .log_const_data :
  {
    PROVIDE(__start_log_const_data = .);
    KEEP(*(SORT(.log_const_data*)))
    PROVIDE(__stop_log_const_data = .);
  } > FLASH
  .log_backends :
  {
    PROVIDE(__start_log_backends = .);
    KEEP(*(SORT(.log_backends*)))
    PROVIDE(__stop_log_backends = .);
  } > FLASH
    .nrf_balloc :
  {
    PROVIDE(__start_nrf_balloc = .);
    KEEP(*(.nrf_balloc))
    PROVIDE(__stop_nrf_balloc = .);
  } > FLASH

} INSERT AFTER .text


INCLUDE "nrf_common.ld"
