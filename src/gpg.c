#include "gpg.h"

void 
error_check(char * logid, gpgme_error_t err) 
{
	if (err != GPG_ERR_NO_ERROR) 
	{
		fprintf(stderr, "%s: %s\n", logid, gpgme_strerror(err));
		exit(err);
	}
}


void init_gpgme(void)
{
	/* init */
	setlocale(LC_ALL, "");
	gpgme_check_version(NULL);
	gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));

}

char * encrypt_text(char *buf, const char *key) 
{

	gpgme_ctx_t ctx;
	gpgme_error_t err;
	gpgme_key_t gkey[2] = { NULL, NULL };
	gpgme_data_t plain, cipher;
	gpgme_encrypt_result_t result;
	int size;
	char *buf_out;
    
    init_gpgme();

	err = gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP);
	error_check("version", err);

	err = gpgme_new(&ctx);
	error_check("ctx", err);
	gpgme_set_armor (ctx, 1);
	gpgme_set_textmode (ctx, 1);
	
	err = gpgme_get_key(ctx, key, &gkey[0], 0);
	error_check("key", err);
	
	err = gpgme_data_new_from_mem(&plain, buf, (size_t) strlen(buf), 0);
	error_check("data", err);

	err = gpgme_data_new(&cipher);
	error_check("cipher", err);
	
	err = gpgme_op_encrypt(ctx, gkey, GPGME_ENCRYPT_ALWAYS_TRUST, plain, cipher);
	error_check("encrypt", err);
	
	result = gpgme_op_encrypt_result(ctx);
	if (result->invalid_recipients) 
	{
		fprintf(stderr, "Invalid recipient %s\n", result->invalid_recipients->fpr);
	}
	
	size = gpgme_data_seek(cipher, 0, SEEK_END);
	buf_out = (char *) malloc(size + 1);

	err = gpgme_data_seek (cipher, 0, SEEK_SET);
	error_check("print", err);
	
	err = gpgme_data_read (cipher, buf_out, size);
	
	if (err < 0)
	  error_check("print", err);

	gpgme_key_unref(gkey[0]);
	gpgme_data_release(plain);
	gpgme_data_release(cipher);
	gpgme_release (ctx);

	return buf_out;
}

char * decrypt_text(char *buf, const char *key) 
{
	gpgme_ctx_t ctx;
	gpgme_error_t err;
	gpgme_data_t plain, cipher;
	gpgme_decrypt_result_t result;
	char *buf_out;
	int size;
	
	init_gpgme();

	err = gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP);
	error_check("version", err);

	err = gpgme_new(&ctx);
	error_check("ctx", err);
	gpgme_set_armor (ctx, 1);
	gpgme_set_textmode (ctx, 1);
	
	err = gpgme_data_new_from_mem(&cipher, buf, (size_t)strlen(buf), 0);
	error_check("data", err);

	err = gpgme_data_new(&plain);
	error_check("cipher", err);

	err = gpgme_op_decrypt(ctx, cipher, plain);
	error_check("decrypt", err);

	result = gpgme_op_decrypt_result(ctx);
	if (result->unsupported_algorithm)
	{
		fprintf(stderr, "%s:%i: unsupported algorithm: %s\n",
		      	__FILE__, __LINE__, result->unsupported_algorithm);
		exit(1);
	}

	size = gpgme_data_seek(plain, 0, SEEK_END);
	buf_out = (char *) malloc(size + 1);

	err = gpgme_data_seek (plain, 0, SEEK_SET);
	error_check("print", err);
	
	err = gpgme_data_read (plain, buf_out, size);
	buf_out[size] = '\0';
	if (err < 0)
	  error_check("print", err);

	gpgme_data_release(plain);
	gpgme_data_release(cipher);
	gpgme_release (ctx);

	return buf_out;
}
