/*--------------------------------------------------------------------------*/
/* Copyright 2022-2024 NXP                                                  */
/*                                                                          */
/* NXP Proprietary. This software is owned or controlled by NXP and may     */
/* only be used strictly in accordance with the applicable license terms.   */
/* By expressly accepting such terms or by downloading, installing,         */
/* activating and/or otherwise using the software, you are agreeing that    */
/* you have read, and that you agree to comply with and are bound by, such  */
/* license terms.  If you do not agree to be bound by the applicable        */
/* license terms, then you may not retain, install, activate or otherwise   */
/* use the software.                                                        */
/*--------------------------------------------------------------------------*/

#include "common.h"

#include <mcuxClToolchain.h>
#include <mcuxClSession.h> // Interface to the entire mcuxClSession component
#include <mcuxCsslFlowProtection.h> // Code flow protection
#include <mcuxClPsaDriver.h>
#include <mcuxClCore_Examples.h>
#include <mcuxClExample_ELS_Helper.h>

/**
 * @brief Example value for RSA key pairs (non-encrypted DER encoding format).
 */
static const ALIGNED uint8_t keyBuffer[] = {
  //SEQUENCE{
  0x30u,
  0x82u, 0x04u, 0xA4u,
  //version           Version,  -- 0
  0x02u,
  0x01u,
  0x00u,
  //modulus           INTEGER,  -- 256-byte
  0x02u,
  0x82u, 0x01u, 0x01u,
  0x00u, 0xBEu, 0xD8u, 0xFFu, 0x2Du, 0xBCu, 0xE9u, 0x6Eu, 0xCBu, 0x7Cu, 0xB6u, 0x86u, 0x86u, 0x6Du, 0x01u, 0x98u,
  0x41u, 0x49u, 0x38u, 0x06u, 0xCAu, 0x50u, 0x8Fu, 0x5Cu, 0xF0u, 0x3Au, 0x02u, 0x90u, 0x90u, 0x5Bu, 0xC5u, 0x1Au,
  0xCCu, 0xE6u, 0x69u, 0x17u, 0xF2u, 0x53u, 0x58u, 0xC0u, 0x94u, 0x93u, 0xEAu, 0x57u, 0x2Bu, 0xC1u, 0x09u, 0x69u,
  0x46u, 0x81u, 0xD3u, 0x15u, 0x4Cu, 0xD5u, 0x23u, 0xBEu, 0x32u, 0x06u, 0xB6u, 0xD0u, 0xEAu, 0x30u, 0xD3u, 0xDDu,
  0x65u, 0x9Bu, 0xE8u, 0xACu, 0xC7u, 0x0Bu, 0x4Cu, 0xA5u, 0x14u, 0xE9u, 0x01u, 0x9Eu, 0x4Eu, 0xEEu, 0x2Fu, 0x57u,
  0x8Au, 0x64u, 0x71u, 0x59u, 0xC9u, 0x4Cu, 0x11u, 0xE2u, 0xE0u, 0xECu, 0xC9u, 0x96u, 0x75u, 0xF4u, 0x92u, 0xDFu,
  0x1Eu, 0x84u, 0x78u, 0xBDu, 0xC4u, 0x3Cu, 0xC1u, 0x03u, 0x8Du, 0x3Cu, 0x4Eu, 0x70u, 0x25u, 0x22u, 0x0Au, 0x15u,
  0x0Au, 0xFFu, 0x9Eu, 0x2Bu, 0x45u, 0x0Cu, 0x72u, 0x11u, 0x0Au, 0xE5u, 0x4Bu, 0x3Cu, 0xCBu, 0x8Au, 0x80u, 0x3Cu,
  0x41u, 0x42u, 0xFEu, 0x78u, 0x34u, 0xF0u, 0x1Au, 0x55u, 0x37u, 0x1Bu, 0x7Du, 0x3Au, 0xEEu, 0x38u, 0x25u, 0x58u,
  0x52u, 0x27u, 0x75u, 0x9Eu, 0x59u, 0x41u, 0xFAu, 0x43u, 0x11u, 0x92u, 0xB9u, 0x70u, 0x17u, 0x1Du, 0x4Bu, 0x11u,
  0xDAu, 0xE0u, 0xF5u, 0xB7u, 0x77u, 0x48u, 0x93u, 0x4Eu, 0x3Bu, 0x68u, 0x60u, 0x08u, 0x86u, 0x57u, 0xD6u, 0x61u,
  0xBFu, 0x4Au, 0x31u, 0x41u, 0xFAu, 0x11u, 0xFBu, 0x3Au, 0x90u, 0x3Au, 0x22u, 0xB8u, 0xE0u, 0x38u, 0x27u, 0xB9u,
  0x25u, 0x8Du, 0x0Eu, 0xDEu, 0x8Au, 0xDCu, 0x65u, 0x04u, 0x7Bu, 0xDFu, 0x4Au, 0xA0u, 0x5Fu, 0x78u, 0x8Fu, 0x7Eu,
  0xC5u, 0x66u, 0xFFu, 0x85u, 0x33u, 0x73u, 0x06u, 0x23u, 0x24u, 0x39u, 0x1Fu, 0x66u, 0x26u, 0x18u, 0x16u, 0x53u,
  0x30u, 0x2Eu, 0x24u, 0xC4u, 0x92u, 0x39u, 0x13u, 0x14u, 0x98u, 0x53u, 0x84u, 0xEAu, 0x99u, 0xDCu, 0x40u, 0x57u,
  0x30u, 0xC4u, 0x2Fu, 0xE7u, 0x89u, 0xB6u, 0x69u, 0x5Du, 0x60u, 0x0Fu, 0x4Bu, 0x1Du, 0x66u, 0x54u, 0x22u, 0x8Du,
  0xB1u,
  //publicExponent    INTEGER,  -- 3-byte
  0x02u,
  0x03u,
  0x01u, 0x00u, 0x01u,
  //privateExponent   INTEGER,  -- 256-byte
  0x02u,
  0x82u, 0x01u, 0x00u,
  0x03u, 0x31u, 0x2Du, 0xA2u, 0x23u, 0x60u, 0xEEu, 0x28u, 0x50u, 0xD2u, 0x31u, 0x66u, 0xD4u, 0x87u, 0x97u, 0x21u,
  0xD9u, 0xBFu, 0xD4u, 0xFBu, 0xE8u, 0xF9u, 0x5Fu, 0x8Fu, 0x14u, 0x66u, 0xA5u, 0x94u, 0xB1u, 0xE1u, 0x96u, 0x9Au,
  0x00u, 0x6Du, 0x42u, 0x92u, 0xC6u, 0xDFu, 0xC5u, 0xA7u, 0x81u, 0x35u, 0x30u, 0x0Cu, 0x67u, 0x22u, 0xA9u, 0x54u,
  0x48u, 0xBDu, 0xF1u, 0xEDu, 0xC2u, 0x8Fu, 0x8Cu, 0xA7u, 0x59u, 0x38u, 0x66u, 0x94u, 0x7Cu, 0x33u, 0x96u, 0xFBu,
  0x93u, 0x98u, 0xD7u, 0xDCu, 0x4Eu, 0x6Du, 0x3Eu, 0x42u, 0x49u, 0x71u, 0x6Cu, 0x51u, 0xEFu, 0xFCu, 0x47u, 0xBAu,
  0x64u, 0xB0u, 0x06u, 0xABu, 0x43u, 0x6Eu, 0xA0u, 0x7Du, 0x02u, 0xF3u, 0x8Eu, 0x3Au, 0x33u, 0xD8u, 0x89u, 0xB5u,
  0xD6u, 0x21u, 0x0Fu, 0x8Au, 0x2Au, 0xE5u, 0x4Eu, 0xE7u, 0x66u, 0x5Au, 0x53u, 0x82u, 0xDEu, 0x27u, 0xB8u, 0x04u,
  0x0Du, 0x3Eu, 0xCFu, 0xDAu, 0x21u, 0xA3u, 0xFEu, 0x1Du, 0x50u, 0xB4u, 0xD4u, 0xF3u, 0xFCu, 0x96u, 0xE4u, 0xD5u,
  0xBFu, 0xD0u, 0x76u, 0x79u, 0xC6u, 0xCCu, 0x23u, 0x44u, 0xD5u, 0xFAu, 0x28u, 0xCEu, 0x5Eu, 0x21u, 0xB0u, 0x2Cu,
  0x4Eu, 0xE4u, 0x8Eu, 0x01u, 0x19u, 0x4Eu, 0xABu, 0x60u, 0x1Bu, 0x42u, 0x3Au, 0x7Bu, 0x90u, 0x1Cu, 0x8Bu, 0x73u,
  0x98u, 0xABu, 0xFEu, 0xF1u, 0xA7u, 0xC2u, 0x1Eu, 0x97u, 0x9Fu, 0x80u, 0x4Eu, 0xCBu, 0xABu, 0xBFu, 0x01u, 0x9Eu,
  0x6Eu, 0x24u, 0x51u, 0x3Eu, 0x01u, 0x4Eu, 0xAAu, 0xC0u, 0x9Bu, 0xD5u, 0xD9u, 0x6Fu, 0x85u, 0x9Cu, 0xAEu, 0x75u,
  0xCEu, 0x7Du, 0x05u, 0xF9u, 0x58u, 0xFFu, 0xB4u, 0x87u, 0xAAu, 0xC4u, 0xFAu, 0xA9u, 0xEBu, 0xC2u, 0x72u, 0x09u,
  0x77u, 0x7Fu, 0x1Du, 0x89u, 0x4Bu, 0x23u, 0x77u, 0x90u, 0x73u, 0x4Fu, 0xB1u, 0xBFu, 0xB1u, 0x5Eu, 0x26u, 0xEDu,
  0x58u, 0x32u, 0x23u, 0x91u, 0x08u, 0xBEu, 0xC9u, 0xD7u, 0x6Fu, 0x9Cu, 0x32u, 0xB9u, 0x74u, 0x29u, 0xF8u, 0x70u,
  0x8Au, 0x84u, 0x18u, 0xAFu, 0xECu, 0x0Au, 0xA6u, 0xF0u, 0x81u, 0x98u, 0x98u, 0x6Cu, 0xE6u, 0x4Bu, 0xF5u, 0xDFu,
  //prime1            INTEGER,  -- 128-byte
  0x02u,
  0x81u, 0x81u,
  0x00u, 0xC1u, 0x26u, 0x5Cu, 0x6Bu, 0xD6u, 0x5Cu, 0x5Du, 0x57u, 0xBFu, 0xA9u, 0x60u, 0x2Eu, 0xCAu, 0x66u, 0x30u,
  0x46u, 0xD8u, 0x2Au, 0x16u, 0x1Eu, 0xEAu, 0xB3u, 0xD7u, 0xF2u, 0x15u, 0xABu, 0x39u, 0xD4u, 0x9Bu, 0xFCu, 0x4Au,
  0xB3u, 0x67u, 0x8Au, 0xC0u, 0x17u, 0xE7u, 0x43u, 0x6Bu, 0x3Du, 0xF1u, 0xB3u, 0xA3u, 0x31u, 0x13u, 0x21u, 0x3Bu,
  0x98u, 0x53u, 0x14u, 0x73u, 0x7Du, 0x10u, 0xEBu, 0x72u, 0x3Eu, 0x2Eu, 0x08u, 0xC8u, 0xC9u, 0x57u, 0xC7u, 0x45u,
  0xDFu, 0x5Du, 0xD5u, 0x6Eu, 0xF4u, 0xABu, 0x99u, 0x66u, 0x8Cu, 0x5Fu, 0x48u, 0xF0u, 0xD4u, 0x95u, 0xF2u, 0xEBu,
  0xCBu, 0x73u, 0x7Fu, 0x70u, 0x69u, 0x6Eu, 0x81u, 0x5Du, 0x86u, 0xACu, 0xFBu, 0xBDu, 0x02u, 0x97u, 0x5Bu, 0xD3u,
  0xEBu, 0x3Au, 0x4Du, 0xBCu, 0x51u, 0xF5u, 0xA9u, 0x9Bu, 0xC0u, 0xB4u, 0xFCu, 0x6Cu, 0xF9u, 0xE2u, 0xC6u, 0xCAu,
  0x5Au, 0x42u, 0x6Bu, 0x82u, 0x10u, 0xD8u, 0x47u, 0x8Cu, 0xFCu, 0x9Eu, 0x4Bu, 0x11u, 0x8Au, 0xF3u, 0xE1u, 0x4Eu,
  0x23u,
  //prime2            INTEGER,  -- 128-byte
  0x02u,
  0x81u, 0x81u,
  0x00u, 0xFCu, 0xF2u, 0xDBu, 0xEFu, 0x1Au, 0x9Eu, 0x4Eu, 0xD5u, 0x74u, 0x1Du, 0xF0u, 0x08u, 0x58u, 0xD4u, 0xEBu,
  0xDEu, 0x88u, 0x45u, 0xADu, 0xC0u, 0xD3u, 0xA6u, 0xA2u, 0x36u, 0x93u, 0xE7u, 0x3Bu, 0x68u, 0x51u, 0x18u, 0x63u,
  0x16u, 0x79u, 0x8Du, 0x4Fu, 0x08u, 0x2Eu, 0xE1u, 0x7Eu, 0xDCu, 0x6Fu, 0x41u, 0x53u, 0x64u, 0xF1u, 0xE0u, 0x3Au,
  0xDFu, 0xD4u, 0x7Du, 0x98u, 0xF8u, 0x93u, 0x23u, 0xEEu, 0x52u, 0xC4u, 0x2Eu, 0x31u, 0x50u, 0xFAu, 0x68u, 0x73u,
  0xA0u, 0x93u, 0xAFu, 0xCFu, 0xA4u, 0x21u, 0xAEu, 0x43u, 0x0Au, 0x3Fu, 0x97u, 0xCAu, 0x58u, 0x61u, 0x60u, 0xB7u,
  0xE5u, 0x78u, 0x35u, 0xD8u, 0xACu, 0x6Fu, 0x11u, 0xBEu, 0x96u, 0xEBu, 0xA9u, 0xA9u, 0x0Cu, 0x5Au, 0xE4u, 0x63u,
  0x48u, 0xBDu, 0x00u, 0x26u, 0xEBu, 0xD7u, 0xDEu, 0x6Au, 0xBDu, 0x0Bu, 0xB8u, 0xA3u, 0x8Au, 0x34u, 0x12u, 0x88u,
  0xC9u, 0x84u, 0x4Du, 0xD3u, 0xA9u, 0x0Au, 0x5Eu, 0xEDu, 0xA9u, 0x2Fu, 0x1Eu, 0x2Bu, 0x09u, 0x2Du, 0x10u, 0x70u,
  0x1Bu,
  //exponent1         INTEGER,  -- 128-byte
  0x02u,
  0x81u, 0x81u,
  0x00u, 0x82u, 0xABu, 0x62u, 0x21u, 0x2Eu, 0x5Fu, 0x44u, 0x62u, 0xE5u, 0xEEu, 0x3Fu, 0x7Cu, 0xC8u, 0x3Fu, 0x03u,
  0xF0u, 0x19u, 0xB3u, 0xB7u, 0x4Du, 0x69u, 0x39u, 0x0Cu, 0x21u, 0xE1u, 0xD8u, 0xFAu, 0x01u, 0xC5u, 0x19u, 0x94u,
  0xABu, 0xF4u, 0xA3u, 0xA0u, 0xBBu, 0x4Bu, 0x20u, 0x88u, 0x3Fu, 0xDAu, 0xF1u, 0xCDu, 0xB8u, 0x98u, 0x99u, 0x86u,
  0x08u, 0xD2u, 0x43u, 0xE6u, 0xB1u, 0xB8u, 0xADu, 0xA0u, 0x97u, 0x42u, 0x6Bu, 0x7Cu, 0xF3u, 0x01u, 0xE8u, 0x75u,
  0x73u, 0xDCu, 0xB6u, 0x55u, 0x1Fu, 0x3Fu, 0xACu, 0x42u, 0xFDu, 0x3Au, 0x45u, 0x4Du, 0x70u, 0x74u, 0x95u, 0x68u,
  0x42u, 0x36u, 0xBCu, 0x03u, 0x9Fu, 0xC0u, 0x3Bu, 0xD2u, 0xBBu, 0x16u, 0xF2u, 0x23u, 0xF7u, 0xC9u, 0xD0u, 0x3Cu,
  0xF9u, 0x49u, 0x73u, 0x67u, 0xB1u, 0x07u, 0x02u, 0x9Cu, 0xB5u, 0x6Du, 0x7Bu, 0xCCu, 0x79u, 0xEDu, 0x9Au, 0xD1u,
  0x30u, 0xE8u, 0xF8u, 0x74u, 0x80u, 0xD2u, 0xE0u, 0xEDu, 0x17u, 0xC6u, 0x3Bu, 0x40u, 0xFEu, 0x01u, 0x69u, 0xEEu,
  0x83u,
  //exponent2         INTEGER,  -- 128-byte
  0x02u,
  0x81u, 0x81u,
  0x00u, 0xB0u, 0xBEu, 0x7Du, 0xA1u, 0x10u, 0x07u, 0x67u, 0xECu, 0x4Cu, 0x6Bu, 0x92u, 0xCAu, 0x32u, 0x4Fu, 0xECu,
  0xD4u, 0x1Cu, 0x82u, 0x1Bu, 0x8Bu, 0xAEu, 0x18u, 0x34u, 0x26u, 0x50u, 0xA8u, 0x74u, 0xE1u, 0x4Au, 0x30u, 0xF1u,
  0x23u, 0xC6u, 0x21u, 0x50u, 0x04u, 0xD6u, 0xC5u, 0x27u, 0xA0u, 0x9Du, 0x78u, 0x96u, 0xEDu, 0xE4u, 0xF8u, 0x9Au,
  0x0Au, 0xC6u, 0x6Eu, 0x50u, 0x51u, 0xF8u, 0x76u, 0x55u, 0xD3u, 0xADu, 0x52u, 0xDDu, 0x90u, 0xC8u, 0xB7u, 0xEDu,
  0x7Bu, 0x59u, 0x56u, 0xB2u, 0x8Eu, 0xECu, 0x1Du, 0xD8u, 0xA8u, 0x33u, 0x91u, 0x3Bu, 0x89u, 0x0Fu, 0xD9u, 0xC6u,
  0x05u, 0x68u, 0x3Eu, 0xAFu, 0xBCu, 0xA5u, 0x0Bu, 0x50u, 0x12u, 0x22u, 0x6Eu, 0xF5u, 0x39u, 0x35u, 0xD5u, 0x79u,
  0xEEu, 0x5Cu, 0x69u, 0xDBu, 0xC8u, 0x55u, 0x99u, 0x0Bu, 0x1Au, 0x37u, 0x33u, 0x77u, 0xCAu, 0x5Cu, 0xE2u, 0x4Au,
  0x84u, 0x0Cu, 0x97u, 0x58u, 0xFBu, 0x37u, 0xCCu, 0xE6u, 0xE1u, 0x9Du, 0x93u, 0xC5u, 0xDCu, 0x6Eu, 0x89u, 0x9Au,
  0xDBu,
  //coefficient       INTEGER,  -- 128-byte
  0x02u,
  0x81u, 0x80u,
  0x66u, 0xB2u, 0x11u, 0x6Fu, 0x95u, 0xF8u, 0x21u, 0x42u, 0xC3u, 0xAEu, 0x71u, 0xBDu, 0x49u, 0x1Du, 0x2Eu, 0xF9u,
  0x8Du, 0xE8u, 0xEFu, 0xBEu, 0x98u, 0xB3u, 0xD2u, 0x36u, 0xD5u, 0x34u, 0x48u, 0x2Bu, 0xF8u, 0x3Eu, 0xB1u, 0x85u,
  0xF4u, 0x87u, 0x3Bu, 0x16u, 0xD3u, 0xEEu, 0x2Cu, 0xCEu, 0xA9u, 0x05u, 0xDBu, 0x59u, 0x0Fu, 0x73u, 0x5Cu, 0x33u,
  0xEAu, 0x70u, 0xF7u, 0xF3u, 0xF6u, 0x88u, 0x7Cu, 0xC1u, 0x1Du, 0x87u, 0xDDu, 0xA0u, 0x33u, 0x1Cu, 0xAEu, 0x6Du,
  0x08u, 0xA4u, 0x5Cu, 0x3Fu, 0x41u, 0x5Cu, 0x1Cu, 0x18u, 0x7Cu, 0xB8u, 0x45u, 0x53u, 0x57u, 0x9Au, 0x91u, 0x1Fu,
  0x41u, 0xF9u, 0x1Du, 0x9Au, 0x9Au, 0x1Eu, 0x1Du, 0xFCu, 0x75u, 0x36u, 0x42u, 0xE5u, 0x6Bu, 0x21u, 0x9Cu, 0x67u,
  0xF2u, 0x66u, 0xFBu, 0x62u, 0xC4u, 0xE9u, 0xF8u, 0x51u, 0x1Du, 0xD9u, 0xBDu, 0xB8u, 0x25u, 0xD8u, 0xE5u, 0x60u,
  0x9Du, 0x3Cu, 0xA1u, 0xDEu, 0x05u, 0xDCu, 0x29u, 0x2Cu, 0x4Au, 0x55u, 0xEDu, 0xF6u, 0xADu, 0xF2u, 0xC4u, 0xDFu
  };

/**
 * @brief Hash of message to be signed
 */
static const ALIGNED uint8_t hash[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {
  0x89u, 0x01u, 0x41u, 0x9fu, 0x26u, 0x14u, 0xc9u, 0x42u, 0xc9u, 0xeeu, 0x5eu, 0xfbu, 0xdfu, 0xbau, 0x0cu, 0xcau,
  0x70u, 0x6bu, 0x3au, 0x4eu, 0xd1u, 0xa8u, 0x5fu, 0x69u, 0x28u, 0xb7u, 0x60u, 0xffu, 0x1bu, 0xbau, 0xb0u, 0xe7u
};

/* Example of RSA PSS signature generation and verification for 2048-bit key and SHA-256 */
MCUXCLEXAMPLE_FUNCTION(mcuxClPsaDriver_rsa_PSS_sign_verify_hash_example)
{
  /**
   * @brief Signature
   */
  MCUX_CSSL_ANALYSIS_START_SUPPRESS_CONTROLLING_EXPRESSION_IS_INVARIANT("External macro")
  ALIGNED uint8_t signature[PSA_SIGN_OUTPUT_SIZE(PSA_KEY_TYPE_RSA_KEY_PAIR, 2048u, PSA_ALG_RSA_PSS_BASE)];
  MCUX_CSSL_ANALYSIS_STOP_SUPPRESS_CONTROLLING_EXPRESSION_IS_INVARIANT()

  /** Initialize ELS, Enable the ELS **/
  if(!mcuxClExample_Els_Init(MCUXCLELS_RESET_DO_NOT_CANCEL))
  {
      return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /*
   * Sign hash: RSASSA-PSS, SHA_256
   */

  /* Set up PSA key attributes. */
  const psa_key_attributes_t sign_attributes = {
    .core = {                                                                                                               // Core attributes
      .type = PSA_KEY_TYPE_RSA_KEY_PAIR,                                                                                    // RSA key pair
      .bits = 2048u,                                                                                                        // Key bits
      .lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_VOLATILE, PSA_KEY_LOCATION_LOCAL_STORAGE),// Volatile (RAM), Local Storage for key pair
      .id = 0U,                                                                                                             // ID zero
      .policy = {
        .usage = PSA_KEY_USAGE_SIGN_HASH,                                                                                   // Key may be used for signing a hash
        .alg = PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),                                                                            // RSA PKCS#1 v1.5 signature with hashing
        .alg2 = PSA_ALG_NONE},
      .flags = 0U},                                                                                                         // No flags
    .domain_parameters = NULL,
    .domain_parameters_size = 0U};

  size_t signature_length;

  psa_status_t sign_status = psa_driver_wrapper_sign_hash(
    &sign_attributes,                   //const psa_key_attributes_t *attributes,
    keyBuffer,                          //const uint8_t *key_buffer,
    sizeof(keyBuffer),                  //size_t key_buffer_size,
    PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),   //psa_algorithm_t alg,
    hash,                               //const uint8_t *hash,
    sizeof(hash),                       //size_t hash_length,
    signature,                          //uint8_t *signature,
    sizeof(signature),                  //size_t signature_size,
    &signature_length                   //size_t *signature_length
    );

  /* Check the return value */
  if(sign_status != PSA_SUCCESS)
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /* Check the signature length */
  MCUX_CSSL_ANALYSIS_START_PATTERN_EXTERNAL_MACRO()
  if(signature_length != PSA_SIGN_OUTPUT_SIZE(PSA_KEY_TYPE_RSA_KEY_PAIR, 2048u, PSA_ALG_RSA_PSS_BASE))
  MCUX_CSSL_ANALYSIS_STOP_PATTERN_EXTERNAL_MACRO()
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /*
   * Verify hash: RSASSA-PSS, SHA_256
   */

  /* Set up PSA key attributes. */
  const psa_key_attributes_t verify_attributes = {
    .core = {                                                                                                               // Core attributes
      .type = PSA_KEY_TYPE_RSA_KEY_PAIR,                                                                                    // RSA key pair
      .bits = 2048u,                                                                                                        // Key bits
      .lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_VOLATILE, PSA_KEY_LOCATION_LOCAL_STORAGE),// Volatile (RAM), Local Storage for key pair
      .id = 0U,                                                                                                             // ID zero
      .policy = {
        .usage = PSA_KEY_USAGE_VERIFY_HASH,                                                                                 // Key may be used for verify a hash
        .alg = PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),                                                                            // RSA PKCS#1 v1.5 signature with hashing
        .alg2 = PSA_ALG_NONE},
      .flags = 0U},                                                                                                         // No flags
    .domain_parameters = NULL,
    .domain_parameters_size = 0U};

  psa_status_t verify_status = psa_driver_wrapper_verify_hash(
    &verify_attributes,                 //const psa_key_attributes_t *attributes,
    keyBuffer,                          //const uint8_t *key_buffer,
    sizeof(keyBuffer),                  //size_t key_buffer_size,
    PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),   //psa_algorithm_t alg,
    hash,                               //const uint8_t *hash,
    sizeof(hash),                       //size_t hash_length,
    signature,                          //const uint8_t *signature,
    sizeof(signature)                   //size_t signature_length
    );

  /* Check the return value */
  if(verify_status != PSA_SUCCESS)
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /* Disable the ELS */
  if(!mcuxClExample_Els_Disable())
  {
    return MCUXCLEXAMPLE_STATUS_ERROR;
  }

  /* Return */
  return MCUXCLEXAMPLE_STATUS_OK;
}

