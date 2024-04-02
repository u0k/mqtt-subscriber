#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include "message_handler.h"

char *payload_text;

struct upload_status {
	size_t bytes_read;
};
/**
 * payload_source() - Read callback function
 * @arg1: Empty char pointer
 * @arg2: Size of read data
 * @arg3: Size of data members
 * @arg4: User provided data
 *
 * Return: Size of data left
 */
static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
	struct upload_status *upload_ctx = (struct upload_status *)userp;
	const char *data;
	size_t room = size * nmemb;

	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}

	data = &payload_text[upload_ctx->bytes_read];

	if (data) {
		size_t len = strlen(data);
		if (room < len)
			len = room;
		memcpy(ptr, data, len);
		upload_ctx->bytes_read += len;

		return len;
	}

	return 0;
}

/**
 * send_email() - Email sending function
 * @arg1: Email of the sender
 * @arg2: Email of the recipient
 * @arg3: App password for GMAIL SMTP
 * @arg4: Topic of the event
 *
 * Return: 0 on success, error code on failure
 */
int send_email(char *sender_email, char *recipient_email, char *app_pw, char *topic, char *additional_data)
{
	if (sender_email == NULL || recipient_email == NULL || topic == NULL)
		return ERROR_SEND_EMAIL;

	int rt_sprintf;
	CURL *curl;
	CURLcode res		      = CURLE_OK;
	struct curl_slist *recipients = NULL;
	curl			      = curl_easy_init();
	int payload_size;
	struct upload_status upload_ctx = { 0 };
	char *payload			= "To: %s\r\n"
					  "From:  %s\r\n"
					  "Subject: Event Trigger\r\n"
					  "\r\n"
					  "Triggered event: \r\n%s\r\n";

	payload_size = snprintf(NULL, 0, payload, recipient_email, sender_email, additional_data);
	payload_text = (char *)malloc(payload_size + 1);
	if (payload_text == NULL) {
		res = ERROR_MALLOC;
		goto cleanup;
	}
	rt_sprintf = sprintf(payload_text, payload, recipient_email, sender_email, additional_data);
	if (rt_sprintf < 0) {
		res = ERROR_ALLOCATING_PAYLOAD;
		goto cleanup;
	}

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_USERNAME, sender_email);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, app_pw);
		curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
		curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, sender_email);
		recipients = curl_slist_append(recipients, recipient_email);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			res = ERROR_SEND_EMAIL;

		curl_slist_free_all(recipients);
		curl_easy_cleanup(curl);
	} else
		res = ERROR_CURL_INIT;

cleanup:
	free(payload_text);
	return (int)res;
}