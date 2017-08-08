/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : MIME
 #	author : miyako
 #	2017/05/12
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

typedef struct
{
	JSONNODE *json;
	PA_Variable *array_blob;
	const wchar_t *name;
}mime_ctx;

#pragma mark JSON

void json_wconv(const char *value, std::wstring &u32)
{
	if(value)
	{
		C_TEXT t;
		CUTF8String u8;
		
		u8 = (const uint8_t *)value;
		t.setUTF8String(&u8);
		
#if VERSIONWIN
		u32 = std::wstring((wchar_t *)t.getUTF16StringPtr());
#else
		
		uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
		std::vector<char> buf(dataSize);
		
		PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
															 t.getUTF16Length() * sizeof(PA_Unichar),
															 eVTC_UTF_16,
															 (char *)&buf[0],
															 dataSize,
															 eVTC_UTF_32);
		
		u32 = std::wstring((wchar_t *)&buf[0]);
#endif
	}else
	{
		u32 = L"";
	}
	
}

JSONNODE *json_parse_text_param(C_TEXT &t)
{
	std::wstring u32;
	
#if VERSIONWIN
	u32 = std::wstring((wchar_t *)t.getUTF16StringPtr());
#else
	
	uint32_t dataSize = (t.getUTF16Length() * sizeof(wchar_t))+ sizeof(wchar_t);
	std::vector<char> buf(dataSize);
	
	PA_ConvertCharsetToCharset((char *)t.getUTF16StringPtr(),
														 t.getUTF16Length() * sizeof(PA_Unichar),
														 eVTC_UTF_16,
														 (char *)&buf[0],
														 dataSize,
														 eVTC_UTF_32);
	
	u32 = std::wstring((wchar_t *)&buf[0]);
#endif
	return json_parse((json_const json_char *)u32.c_str());
}

void json_set_text_param(JSONNODE *n, C_TEXT &t)
{
	if(n)
	{
		json_char *json_string = json_write_formatted(n);
		
		std::wstring wstr = std::wstring(json_string);
		
#if VERSIONWIN
		t.setUTF16String((const PA_Unichar *)wstr.c_str(), (uint32_t)wstr.length());
#else
		uint32_t dataSize = (uint32_t)((wstr.length() * sizeof(wchar_t)) + sizeof(PA_Unichar));
		std::vector<char> buf(dataSize);
		
		uint32_t len = PA_ConvertCharsetToCharset((char *)wstr.c_str(),
																							(PA_long32)(wstr.length() * sizeof(wchar_t)),
																							eVTC_UTF_32,
																							(char *)&buf[0],
																							dataSize,
																							eVTC_UTF_16);
		
		t.setUTF16String((const PA_Unichar *)&buf[0], len);
#endif
		
		json_free(json_string);
	}
	
}

void json_set_object(JSONNODE *n, const wchar_t *name, JSONNODE *o)
{
	json_set_name(o, (json_const json_char *)name);
	json_push_back(n, o);
}

BOOL json_get_string(JSONNODE *json, CUTF8String &value)
{
	value = (const uint8_t *)"";
	
	if(json)
	{
		if(json_type(json) == JSON_STRING)
		{
			json_char *s = json_as_string(json);
			
			std::wstring wstr = std::wstring(s);
			
			C_TEXT t;
			
#if VERSIONWIN
			t.setUTF16String((const PA_Unichar *)wstr.c_str(), (uint32_t)wstr.length());
#else
			uint32_t dataSize = (uint32_t)((wstr.length() * sizeof(wchar_t)) + sizeof(PA_Unichar));
			std::vector<char> buf(dataSize);
			
			uint32_t len = PA_ConvertCharsetToCharset((char *)wstr.c_str(),
																								(PA_long32)(wstr.length() * sizeof(wchar_t)),
																								eVTC_UTF_32,
																								(char *)&buf[0],
																								dataSize,
																								eVTC_UTF_16);
			
			t.setUTF16String((const PA_Unichar *)&buf[0], len);
#endif
			
			t.copyUTF8String(&value);
			
			json_free(s);
		}
		
	}
	
	return !!value.length();
}

BOOL json_get_string(JSONNODE *json, const wchar_t *name, CUTF8String &value)
{
	value = (const uint8_t *)"";
	
	if(json)
	{
		JSONNODE *node = json_get(json, name);
		if(node)
		{
			json_char *s =json_as_string(node);
			
			std::wstring wstr = std::wstring(s);
			
			C_TEXT t;
			
#if VERSIONWIN
			t.setUTF16String((const PA_Unichar *)wstr.c_str(), (uint32_t)wstr.length());
#else
			uint32_t dataSize = (uint32_t)((wstr.length() * sizeof(wchar_t)) + sizeof(PA_Unichar));
			std::vector<char> buf(dataSize);
			
			uint32_t len = PA_ConvertCharsetToCharset((char *)wstr.c_str(),
																								(PA_long32)(wstr.length() * sizeof(wchar_t)),
																								eVTC_UTF_32,
																								(char *)&buf[0],
																								dataSize,
																								eVTC_UTF_16);
			
			t.setUTF16String((const PA_Unichar *)&buf[0], len);
#endif
			
			t.copyUTF8String(&value);
			
			json_free(s);
		}
	}
	
	return !!value.length();
}

void json_set_text(JSONNODE *n, const wchar_t *name, char *value, BOOL optional, BOOL release)
{
	if(n)
	{
		if(value)
//		if((!optional) || ((optional) && (value)))
		{
			std::wstring w32;
			json_wconv(value, w32);
			json_push_back(n, json_new_a(name, w32.c_str()));
			
			if(release)
			{
				g_free(value);
			}
		}else
		{
			JSONNODE *node = json_new_a(name, L"");
			json_nullify(node);
			json_push_back(n, node);
		}
	}
}

void json_set_date(JSONNODE *n, GDateTime *dt, const wchar_t *date, const wchar_t *time, const char *fmt)
{
	if(n)
	{
		json_set_number(n, time, (json_int_t)(((g_date_time_get_hour(dt) * 3600)
																					+ (g_date_time_get_minute(dt) * 60)
																					+ (g_date_time_get_second(dt))) * 1000));
		
		gchar *dateString = g_date_time_format(dt, fmt);
		
		json_set_text(n, date, dateString);
		
#if VERSIONMAC
		if(dateString)
		{
			g_free(dateString);//corrupts heap on windows
		}
#endif
		
		g_date_time_unref(dt);
	}
	
}

void json_set_number(JSONNODE *n, const wchar_t *name, json_int_t value)
{
	if(n)
	{
		json_push_back(n, json_new_i(name, value));
	}
}

#pragma mark Startup / Exit

bool IsProcessOnExit()
{
	C_TEXT name;
	PA_long32 state, time;
	PA_GetProcessInfo(PA_GetCurrentProcessNumber(), name, &state, &time);
	CUTF16String procName(name.getUTF16StringPtr());
	CUTF16String exitProcName((PA_Unichar *)"$\0x\0x\0\0\0");
	return (!procName.compare(exitProcName));
}

void OnExit()
{
	g_mime_shutdown();
}

void OnCloseProcess()
{
	if(IsProcessOnExit())
	{
		OnExit();
	}
}

void OnStartup()
{
	g_mime_init();
}

#pragma mark -

void PluginMain(PA_long32 selector, PA_PluginParameters params)
{
	try
	{
		PA_long32 pProcNum = selector;
		
		switch(pProcNum)
		{
			case kInitPlugin :
			case kServerInitPlugin :
				OnStartup();
				break;
				
			case kCloseProcess :
				OnCloseProcess();
				break;
				
			case 1 :
				MIME_PARSE_MESSAGE(params);
				break;
				
			case 2 :
				MIME_Create_message(params);
				break;
		}
	}
	catch(...)
	{
		
	}
}

// ------------------------------- Parsing Messages -------------------------------

#pragma mark MIME subobjects

void getHeaders(GMimeObject *part, const wchar_t *label, JSONNODE *json_message)
{
	GMimeHeaderList *headers = g_mime_object_get_header_list (part);
	
	if(headers)
	{
		int len = g_mime_header_list_get_count(headers);
		
		if(len)
		{
			JSONNODE *header_array = json_new(JSON_ARRAY);
			
			for(int i = 0; i < len; ++i)
			{
				GMimeHeader *h = g_mime_header_list_get_header_at(headers, i);
				
				JSONNODE *item = json_new(JSON_NODE);
				
				json_set_text(item, L"name", (char *)g_mime_header_get_name(h));
				json_set_text(item, L"value", g_mime_utils_header_decode_text(NULL, g_mime_header_get_value(h)));
				
				json_push_back(header_array, item);
			}
			json_set_object(json_message, label, header_array);
		}
		g_mime_header_list_clear(headers);
	}
}

void getAddress(InternetAddressList *list, const wchar_t *label, JSONNODE *json_message)
{
	InternetAddress *address;
	
	if(list)
	{
		int len = internet_address_list_length(list);
		
		if(len)
		{
			JSONNODE *address_array = json_new(JSON_ARRAY);
			
			for(int i = 0; i < len; ++i)
			{
				address = internet_address_list_get_address(list, i);
				
				JSONNODE *item = json_new(JSON_NODE);
				//internet_address_to_string will alloc
				// https://developer.gnome.org/gmime/stable/InternetAddress.html#internet-address-to-string
				
				json_set_text(item, L"string", internet_address_to_string(address, NULL, FALSE), FALSE, TRUE);
				json_set_text(item, L"encoded_string", internet_address_to_string(address, NULL, TRUE), FALSE, TRUE);
				
				json_set_text(item, L"addr", (char *)internet_address_mailbox_get_addr((InternetAddressMailbox *)address));
				json_set_text(item, L"idn_addr", (char *)internet_address_mailbox_get_idn_addr ((InternetAddressMailbox *)address));
				json_set_text(item, L"name", (char *)internet_address_get_name(address));
				
				json_push_back(address_array, item);
			}
			json_set_object(json_message, label, address_array);
		}else
		{
			//create an empty array
			json_set_object(json_message, label, json_new(JSON_ARRAY));
		}
		internet_address_list_clear(list);
	}else
	{
		//create an empty array
		json_set_object(json_message, label, json_new(JSON_ARRAY));
	}
	
}

void processTopLevel(GMimeObject *parent, GMimeObject *part, gpointer user_data)
{
	mime_ctx *ctx = (mime_ctx *)user_data;
	
	bool isPart = GMIME_IS_PART(part);
	
	if (isPart)
	{
		ctx->name = g_mime_part_is_attachment((GMimePart *)part) ? L"attachments" : L"body";
		
		if(GMIME_IS_MULTIPART(part))
		{
			g_mime_multipart_foreach((GMimeMultipart *)part, processNextLevel, ctx);
		}
		else
		{
			processNextLevel(parent, part, ctx);
		}
		
	}//isPart
	
	getHeaders(part, L"headers", ctx->json);
}

void processNextLevel(GMimeObject *parent, GMimeObject *part, gpointer user_data)
{
	mime_ctx *ctx = (mime_ctx *)user_data;
	
	if(GMIME_IS_MULTIPART(part))
	{
		g_mime_multipart_foreach((GMimeMultipart *)part, processNextLevel, user_data);
	}else
	{
		GMimeDataWrapper *wrapper = g_mime_part_get_content((GMimePart *)part);
		if(wrapper)
		{
			JSONNODE *json_part = json_new(JSON_NODE);
			
			GMimeContentType *partMediaType = g_mime_object_get_content_type(part);
			if(0 == strncasecmp(g_mime_content_type_get_media_type(partMediaType), "text", 4))
			{
				//g_mime_text_part_get_text will alloc
				// https://developer.gnome.org/gmime/stable/GMimeTextPart.html
				char *text = g_mime_text_part_get_text((GMimeTextPart *)part);
				json_set_text(json_part, L"data", text, FALSE, TRUE);
				
			}else
			{
				GMimeStream *content = g_mime_stream_mem_new();
				g_mime_data_wrapper_write_to_stream(wrapper, content);
				GByteArray *bytes = g_mime_stream_mem_get_byte_array ((GMimeStreamMem *)content);
				
				PA_long32 i = PA_GetArrayNbElements(*(ctx->array_blob));
				PA_ResizeArray(ctx->array_blob, ++i);

				PA_Variable element = PA_CreateVariable(eVK_Blob);
				PA_SetBlobVariable(&element, (void *)bytes->data, bytes->len);
				PA_SetBlobInArray(*(ctx->array_blob), i, element.uValue.fBlob);
				
				json_set_number(json_part, L"data", i);
				
				g_mime_stream_close(content);
				g_clear_object(&content);
			}
			
			//Content-Type
			GMimeContentType *type = g_mime_object_get_content_type(part);
			
			//g_mime_content_type_get_mime_type will alloc
			// https://developer.gnome.org/gmime/stable/GMimeContentType.html#g-mime-content-type-get-mime-type
			json_set_text(json_part, L"mime_type", (char *)g_mime_content_type_get_mime_type(type), FALSE, TRUE);
			
			json_set_text(json_part, L"media_type", (char *)g_mime_content_type_get_media_type(type));
			json_set_text(json_part, L"media_subtype", (char *)g_mime_content_type_get_media_subtype(type));
			json_set_text(json_part, L"content_id", (char *)g_mime_object_get_content_id(part));
			json_set_text(json_part, L"content_encoding", (char *)g_mime_content_encoding_to_string(g_mime_part_get_content_encoding((GMimePart *)part)));
			
			json_set_text(json_part, L"file_name", (char *)g_mime_part_get_filename((GMimePart *)part), TRUE);
			json_set_text(json_part, L"content_description", (char *)g_mime_part_get_content_description((GMimePart *)part), TRUE);
			json_set_text(json_part, L"content_md5", (char *)g_mime_part_get_content_md5((GMimePart *)part), TRUE);
			json_set_text(json_part, L"content_location", (char *)g_mime_part_get_content_location((GMimePart *)part), TRUE);
			
			getHeaders(part, L"headers", json_part);
			
			JSONNODE *array_part = json_get(ctx->json, ctx->name);
			
			if(json_type(array_part) != JSON_ARRAY)
			{
				array_part = json_new(JSON_ARRAY);
				json_push_back(array_part, json_part);
				json_set_object(ctx->json, ctx->name, array_part);
			}else
			{
				json_push_back(array_part, json_part);
			}
		}
	}
}

void returnBlob(sLONG_PTR *pResult, PackagePtr pParams, void *blob, PA_long32 len)
{
	PluginBlock params;
	params.fResult = pResult;
	params.fParameters = pParams;
	params.fData = NULL;
	PA_ReturnBlob(&params, blob, len);
}

void filter_double_header(GMimeObject *message_mime, const char *name, BOOL with_new_line)
{
	if(name)
	{
		CUTF8String header_string = (const uint8_t *)name;
		if(with_new_line)
		{
			header_string += (const uint8_t *)": \n";
		}else{
			header_string += (const uint8_t *)":";
		}
		
		for (guint i = 0; i < message_mime->headers->array->len; i++)
		{
			GMimeHeader *header = (GMimeHeader *) message_mime->headers->array->pdata[i];
			
			if (!g_ascii_strcasecmp (name, header->name))
			{
				CUTF8String raw_header_value = (const uint8_t *)g_mime_header_get_raw_value (header);
				if(raw_header_value.find(header_string) == 0)
				{
					g_mime_header_set_raw_value(header, (const char *)raw_header_value.substr(header_string.length()).c_str());
					break;
				}
			}
		}
	}
}

void add_mailboxes(GMimeMessage *message, JSONNODE *message_node, const wchar_t *key, GMimeAddressType type)
{
	JSONNODE *node = json_get(message_node, key);
	
	if(node)
	{
		JSONNODE *array_node = json_as_array(node);
		if(array_node)
		{
			json_index_t array_node_count = json_size(array_node);
			for (json_index_t i = 0; i < array_node_count; i++)
			{
				JSONNODE *item_node = json_at(array_node, i);
				if(item_node)
				{
					CUTF8String name, addr;
					json_get_string(item_node, L"name", name);
					json_get_string(item_node, L"addr", addr);
					g_mime_message_add_mailbox(message, type, (const char *)name.c_str(), (const char *)addr.c_str());
				}
			}
		}
	}
}

void add_headers(GMimeObject *message_mime, JSONNODE *message_node)
{
	JSONNODE *node = json_get(message_node, L"headers");
	
	if(node)
	{
		JSONNODE *array_node = json_as_array(node);
		if(array_node)
		{
			json_index_t array_node_count = json_size(array_node);
			for (json_index_t i = 0; i < array_node_count; i++)
			{
				JSONNODE *item_node = json_at(array_node, i);
				if(item_node)
				{
					CUTF8String name, value, header_charset;
					
					json_get_string(item_node, L"name", name);
					json_get_string(item_node, L"value", value);
					const char *charset = NULL;
					if(json_get_string(item_node, L"charset", header_charset))
						charset = (const char *)header_charset.c_str();
					
					g_mime_object_set_header(message_mime, (const char *)name.c_str(), (const char *)value.c_str(), charset);
					filter_double_header(message_mime, (const char *)name.c_str());
					
				}
			}
		}
	}
}

void add_date(GMimeMessage *message, JSONNODE *message_node)
{
	GMimeObject *message_mime = (GMimeObject *)message;
	
	CUTF8String utc_date;
	if(json_get_string(message_node, L"utc_date", utc_date))
	{
		GTimeVal time;
		if(g_time_val_from_iso8601 ((const gchar *)utc_date.c_str(), &time))
		{
			GDateTime *date = g_date_time_new_from_timeval_utc(&time);
			g_mime_message_set_date(message, date);
			g_date_time_unref(date);
			filter_double_header(message_mime, "Date");
		}
	}else
	{
		CUTF8String local_date;
		if(json_get_string(message_node, L"local_date", local_date))
		{
			GTimeVal time;
			if(g_time_val_from_iso8601 ((const gchar *)local_date.c_str(), &time))
			{
				GDateTime *date = g_date_time_new_from_timeval_local(&time);
				g_mime_message_set_date(message, date);
				g_date_time_unref(date);
				filter_double_header(message_mime, "Date");
			}
		}
	}
}

void add_parts(GMimeObject *message_mime, JSONNODE *message_node, PA_Variable *data_array)
{
	JSONNODE *node = json_get(message_node, L"body");
	
	if(node)
	{
		JSONNODE *array_node = json_as_array(node);
		if(array_node)
		{
			json_index_t array_node_count = json_size(array_node);
			
			GMimeMultipart *multipart = NULL;
			
			//set the content type based on header value
			if(array_node_count > 1)
			{
				multipart = g_mime_multipart_new();
				g_mime_message_set_mime_part((GMimeMessage *)message_mime, (GMimeObject *)multipart);
				g_mime_object_set_content_type((GMimeObject *)multipart, g_mime_object_get_content_type(message_mime));
			}
			g_mime_object_remove_header (message_mime, "Content-Type");
			
			for (json_index_t i = 0; i < array_node_count; i++)
			{
				JSONNODE *item_node = json_at(array_node, i);
				if(item_node)
				{
					CUTF8String part_data, part_charset, part_text;
					JSONNODE *data_node = json_get(item_node, L"data");
					if(data_node)
					{
						GMimePart *part = NULL;
						GMimeTextPart *text_part = NULL;
						
						if(JSON_STRING == json_type(data_node))
						{
							text_part = g_mime_text_part_new();
							
							if(json_get_string(item_node, L"charset", part_charset))
								g_mime_text_part_set_charset(text_part, (const char *)part_charset.c_str());
							
							if(json_get_string(item_node, L"data", part_text))
								g_mime_text_part_set_text (text_part, (const char *)part_text.c_str());
							
							part = (GMimePart *)text_part;
							
						}else{
							
							part = g_mime_part_new();
							
						}
						
						GMimeObject *part_mime = (GMimeObject *)part;
						add_headers(part_mime, item_node);
						
						//base64 seems more robust for binary parts
						if(JSON_NUMBER == json_type(data_node))
						{
							g_mime_part_set_content_encoding(part, GMIME_CONTENT_ENCODING_BASE64);
						}else
						{
							CUTF8String part_content_encoding;
							if(json_get_string(item_node, L"content_encoding", part_content_encoding))
							{
								GMimeContentEncoding content_encoding = g_mime_content_encoding_from_string((const char *)part_content_encoding.c_str());
								g_mime_part_set_content_encoding(part, content_encoding);
							}else
							{
								//make this the default for text
								g_mime_part_set_content_encoding(part, GMIME_CONTENT_ENCODING_BASE64);
							}
						}
						
						filter_double_header(part_mime, "Content-Transfer-Encoding");
						
						CUTF8String part_content_id;
						if(json_get_string(item_node, L"content_id", part_content_id))
							g_mime_part_set_content_id(part, (const char *)part_content_id.c_str());
						
						CUTF8String part_content_type;
						if(json_get_string(item_node, L"mime_type", part_content_type))
						{
							GMimeContentType *content_type = g_mime_content_type_parse(g_mime_parser_options_get_default(), (const char *)part_content_type.c_str());
							g_mime_object_set_content_type(part_mime, content_type);
						}
						
						CUTF8String part_content_location;
						if(json_get_string(item_node, L"content_location", part_content_location))
						{
							g_mime_part_set_content_location(part, (const char *)part_content_location.c_str());
							filter_double_header(part_mime, "Content-Location");
						}
						
						CUTF8String part_content_description;
						if(json_get_string(item_node, L"content_description", part_content_description))
						{
							g_mime_part_set_content_description(part, (const char *)part_content_description.c_str());
							filter_double_header(part_mime, "Content-Description");
						}
						
						CUTF8String part_file_name;
						if(json_get_string(item_node, L"file_name", part_file_name))
						{
							g_mime_part_set_filename(part, (const char *)part_file_name.c_str());
						}
						
						CUTF8String part_content_md5;
						if(json_get_string(item_node, L"content_md5", part_content_md5))
						{
							g_mime_part_set_content_md5(part, (const char *)part_content_md5.c_str());
							filter_double_header(part_mime, "Content-Md5");
						}
						
						if(JSON_NUMBER == json_type(data_node))
						{
							json_int_t data_index = json_as_int(data_node);
							
							if(PA_GetVariableKind(*data_array) == eVK_ArrayBlob)
							{
								if(PA_GetArrayNbElements(*data_array) >= data_index)
								{
									PA_Blob data = PA_GetBlobInArray(*data_array, (PA_long32)data_index);
									void *data_buf = (void *)PA_LockHandle(data.fHandle);
									
									GMimeStream *stream = g_mime_stream_mem_new_with_buffer((const char *)data_buf, data.fSize);
									GMimeDataWrapper *wrapper = g_mime_data_wrapper_new_with_stream(stream, GMIME_CONTENT_ENCODING_DEFAULT);
									g_mime_part_set_content(part, wrapper);
									
									PA_UnlockHandle(data.fHandle);
									g_object_unref(wrapper);//GMimeDataWrapper
									g_object_unref(stream);//GMimeStream
								}
							}
						}
						
						if(array_node_count == 1)
						{
							g_mime_message_set_mime_part ((GMimeMessage *)message_mime, part_mime);
						}
						else
						{
							g_mime_multipart_add (multipart, part_mime);
						}
						
						g_object_unref (part_mime);
					}
				}
			}
		}
	}
}

#pragma mark -

void MIME_PARSE_MESSAGE(PA_PluginParameters params)
{
	PackagePtr pParams = (PackagePtr)params->fParameters;
	
	JSONNODE *json = json_new(JSON_NODE);
	JSONNODE *json_message = json_new(JSON_NODE);
	
	C_TEXT Param2;
	
	PA_Variable Param3 = PA_CreateVariable(eVK_ArrayBlob);
	
	PA_Handle h = *(PA_Handle *)(pParams[0]);
	
	if(h)
	{
		GMimeStream *stream = g_mime_stream_mem_new_with_buffer((const char *)PA_LockHandle(h), PA_GetHandleSize(h));
		GMimeParser *parser = g_mime_parser_new_with_stream (stream);
		GMimeParserOptions *options = g_mime_parser_options_new();
		g_mime_parser_options_set_address_compliance_mode(options, GMIME_RFC_COMPLIANCE_LOOSE);
		g_mime_parser_options_set_allow_addresses_without_domain(options, true);
		g_mime_parser_options_set_parameter_compliance_mode(options, GMIME_RFC_COMPLIANCE_LOOSE);
		g_mime_parser_options_set_rfc2047_compliance_mode(options, GMIME_RFC_COMPLIANCE_LOOSE);
		GMimeMessage *message = g_mime_parser_construct_message (parser, options);
		g_mime_parser_options_free(options);
		g_clear_object(&parser);
		g_clear_object(&stream);
		
		mime_ctx ctx;
		ctx.json = json_message;
		ctx.array_blob = &Param3;
		ctx.name = L"body";
		
		if(message)
		{
			getAddress(g_mime_message_get_from (message), L"from", json_message);
			getAddress(g_mime_message_get_cc (message), L"cc", json_message);
			getAddress(g_mime_message_get_to (message), L"to", json_message);
			getAddress(g_mime_message_get_bcc (message), L"bcc", json_message);
			getAddress(g_mime_message_get_sender (message), L"sender", json_message);
			getAddress(g_mime_message_get_reply_to (message), L"reply_to", json_message);
			getAddress(g_mime_message_get_all_recipients (message), L"all_recipients", json_message);
			
			json_set_text(json_message, L"id", (char *)g_mime_message_get_message_id(message));
			json_set_text(json_message, L"subject", (char *)g_mime_message_get_subject(message));
			
			GDateTime *date = g_mime_message_get_date(message);
			
			json_set_date(json_message, g_date_time_to_local(date), L"local_date", L"local_time", "%Y-%m-%dT%H:%M:%S%z");
			json_set_date(json_message, g_date_time_to_utc(date), L"utc_date", L"utc_time", "%Y-%m-%dT%H:%M:%SZ");
			
			g_mime_message_foreach(message, processTopLevel, &ctx);
			
			g_clear_object(&message);
		}
		
		json_set_object(json, L"message", json_message);
		json_set_text_param(json, Param2);
		
		json_delete(json);
		
		PA_UnlockHandle(h);
	}
	
	PA_SetVariableParameter(params, 3, Param3, 0);
	
	Param2.toParamAtIndex(pParams, 2);
}

void MIME_Create_message(PA_PluginParameters params)
{
	PackagePtr pParams = (PackagePtr)params->fParameters;
	
	C_TEXT Param1;
	Param1.fromParamAtIndex(pParams, 1);
	
	PA_Variable *data_array_p = ((PA_Variable *)pParams[1]);//Param2
	
	JSONNODE *json = json_parse_text_param(Param1);
	
	if(json)
	{
		GMimeMessage *message = g_mime_message_new(false);
		GMimeObject *message_mime = (GMimeObject *)message;
		
		JSONNODE *message_node = json_get(json, L"message");
		
		if(message_node)
		{
			add_headers(message_mime, message_node);
			add_date(message, message_node);
			add_mailboxes(message, message_node, L"from", GMIME_ADDRESS_TYPE_FROM);
			add_mailboxes(message, message_node, L"cc", GMIME_ADDRESS_TYPE_CC);
			add_mailboxes(message, message_node, L"to", GMIME_ADDRESS_TYPE_TO);
			add_mailboxes(message, message_node, L"bcc", GMIME_ADDRESS_TYPE_BCC);
			add_mailboxes(message, message_node, L"sender", GMIME_ADDRESS_TYPE_SENDER);
			add_mailboxes(message, message_node, L"reply_to", GMIME_ADDRESS_TYPE_REPLY_TO);
			
			CUTF8String message_subject;
			const char *subject = NULL;
			CUTF8String message_subject_charset;
			const char *charset = NULL;
			CUTF8String message_id;
			
			if(json_get_string(message_node, L"subject_charset", message_subject_charset))
				charset = (const char *)message_subject_charset.c_str();
			
			if(json_get_string(message_node, L"subject", message_subject))
			{
				subject = (const char *)message_subject.c_str();
				g_mime_message_set_subject(message, subject, charset);//Subject with capital S
				filter_double_header(message_mime, "Subject");
			}
			
			if(json_get_string(message_node, L"id", message_id))
				g_mime_message_set_message_id(message, (const char *)message_id.c_str());
			
			//over ride header definition
			CUTF8String message_content_type;
			if(json_get_string(message_node, L"mime_type", message_content_type))
			{
				GMimeContentType *content_type = g_mime_content_type_parse(g_mime_parser_options_get_default(), (const char *)message_content_type.c_str());
				g_mime_object_set_content_type(message_mime, content_type);
			}
			
			add_parts(message_mime, message_node, data_array_p);
			
			//prepare
			GMimeFormatOptions *format_options = g_mime_format_options_new();
			g_mime_format_options_set_param_encoding_method(format_options, GMIME_PARAM_ENCODING_METHOD_RFC2231);
			g_mime_format_options_set_newline_format (format_options, GMIME_NEWLINE_FORMAT_DOS);
			GMimeStream *stream = g_mime_stream_mem_new();
			GByteArray *array = g_byte_array_new();
			g_mime_stream_mem_set_byte_array ((GMimeStreamMem *)stream, array);
			
			if(/* DISABLES CODE */ (0))
			{//debug: print headers
				g_mime_header_list_write_to_stream(message_mime->headers, format_options, stream);
			}
			g_mime_object_write_to_stream (message_mime, format_options, stream);
			
			PA_ReturnBlob(params, array->data, array->len);
			
			//cleanup
			g_object_unref (stream);//GMimeStream
			g_byte_array_free (array, FALSE);//GByteArray
			g_mime_format_options_free(format_options);//GMimeFormatOptions
			
		}
		json_delete(json);
	}
	
}
