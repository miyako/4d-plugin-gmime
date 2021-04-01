//%attributes = {}
$path:=Get 4D folder:C485(Current resources folder:K5:16)+"wakanda-mascot-walking.eml"

DOCUMENT TO BLOB:C525($path;$eml)

C_TEXT:C284($mime_json)
ARRAY BLOB:C1222($mime_blobs;0)

MIME PARSE MESSAGE ($eml;$mime_json;$mime_blobs)

$preview:=JSON Parse:C1218($mime_json)

If (True:C214)
	
	  //add an attachment
	
	$path:=Get 4D folder:C485(Current resources folder:K5:16)+"wakanda-mascot-walking.png"
	DOCUMENT TO BLOB:C525($path;$image)
	APPEND TO ARRAY:C911($mime_blobs;$image)
	APPEND TO ARRAY:C911($mime_blobs;$image)
	
	$message:=OB Get:C1224($preview;"message")
	ARRAY OBJECT:C1221($attachments;0)
	
	C_OBJECT:C1216($b)
	
	OB SET:C1220($b;\
		"file_name";"わかんだちゃん.png";\
		"content_description";"わかんだちゃん";\
		"content_location";"http://www.4d.com";\
		"content_md5";Generate digest:C1147("わかんだちゃん";MD5 digest:K66:1);\
		"mime_type";"image/png";\
		"data";1)  //element number of this blob in $2
	APPEND TO ARRAY:C911($attachments;$b)
	
	CLEAR VARIABLE:C89($b)
	
	OB SET:C1220($b;\
		"file_name";"わかんだちゃん2.png";\
		"content_description";"わかんだちゃん";\
		"content_location";"http://www.4d.com";\
		"content_md5";Generate digest:C1147("わかんだちゃん";MD5 digest:K66:1);\
		"mime_type";"image/png";\
		"data";2)  //element number of this blob in $2
	APPEND TO ARRAY:C911($attachments;$b)
	
	OB SET ARRAY:C1227($message;"attachments";$attachments)
	
End if 

$mime_json:=JSON Stringify:C1217($preview)

$eml:=MIME Create message ($mime_json;$mime_blobs)

$path:=System folder:C487(Desktop:K41:16)+"1.eml"
BLOB TO DOCUMENT:C526($path;$eml)

OPEN URL:C673($path)