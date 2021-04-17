/**
 * \file sm4.h
 */
#ifndef XYSSL_SM4_H
#define XYSSL_SM4_H
/*
#define SM4_ENCRYPT     1
#define SM4_DECRYPT     0
  */
 /**
  * \brief          SM4 context structure
  */ /*
typedef struct
{
    unsigned char mode;                  
    unsigned long sk[32];       
}
 sm4_context;
 */
//unsigned long sk[32]; 

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * \brief          SM4 key schedule (128-bit, encryption)
     *
     * \param ctx      SM4 context to be initialized
     * \param key      16-byte secret key
     */
    void sm4_setkey_enc(unsigned long sk[32], unsigned char key[16]);

    /**
     * \brief          SM4 key schedule (128-bit, decryption)
     *
     * \param ctx      SM4 context to be initialized
     * \param key      16-byte secret key
     */
//    void sm4_setkey_dec(sm4_context* ctx, unsigned char key[16]);

    /**
     * \brief          SM4-ECB block encryption/decryption
     * \param ctx      SM4 context
     * \param mode     SM4_ENCRYPT or SM4_DECRYPT
     * \param length   length of the input data
     * \param input    input block
     * \param output   output block
     */
    void sm4_crypt_ecb(unsigned long sk[32],
        unsigned char length,
        unsigned char* input,
        unsigned char* output);

    /**
     * \brief          SM4-CBC buffer encryption/decryption
     * \param ctx      SM4 context
     * \param mode     SM4_ENCRYPT or SM4_DECRYPT
     * \param length   length of the input data
     * \param iv       initialization vector (updated after use)
     * \param input    buffer holding the input data
     * \param output   buffer holding the output data
     */
   /* void sm4_crypt_cbc(sm4_context* ctx,
        unsigned char mode,
        unsigned char length,
        unsigned char iv[16],
        unsigned char* input,
        unsigned char* output);
*/
#ifdef __cplusplus
}
#endif

#endif /* sm4.h */

