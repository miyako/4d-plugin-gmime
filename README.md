# 4d-plugin-gmime

Parse MIME using the [GMIME](https://github.com/GNOME/gmime) library.

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" />

### Releases

[2.0](https://github.com/miyako/4d-plugin-gmime/releases/tag/2.0) Visual C

[1.1](https://github.com/miyako/4d-plugin-gmime/releases/tag/1.1) MinGW

## Syntax

```
MIME PARSE MESSAGE (mime;json;data)
```

Parameter|Type|Description
------------|------------|----
mime|BLOB|Raw MIME data (``.eml`` or ``.mbox``)
json|TEXT|The parsed MIME content
data|ARRAY BLOB|Raw data (images, attachments) linked to the MIME content

```
mime:=MIME Create message (json;data)
```

Parameter|Type|Description
------------|------------|----
json|TEXT|The parsed MIME content
data|ARRAY BLOB|Raw data (images, attachments) linked to the MIME content
mime|BLOB|Raw MIME data

### The parsed MIME content

**Note**: The [JSON schema](https://github.com/miyako/4d-plugin-gmime/blob/master/MIME.4dbase/Resources/schema.json) is attached to the sample database.

Use it like

```
$path:=Get 4D folder(Current resources folder)+"schema.json"
$schema_json:=Document to text($path;"utf-8")
$schema:=JSON Parse($schema_json)
$errors:=JSON Validate($mime;$schema)
```

* Properties of the root ``message`` object

``from`` : ``address`` array (optional)

``cc`` : ``address`` array (optional)

``to`` : ``address`` array (optional)

``bcc`` : ``address`` array (optional)

``sender`` : ``address`` array (optional)

``reply_to`` : ``address`` array (optional)

``all_recipients`` : ``address`` array (optional)

``local_date`` : ``string`` (ISO)

``utc_date`` : ``string`` (ISO)

``local_time`` : ``number`` (milliseconds)

``utc_time`` : ``number`` (milliseconds)

``id`` : ``string``

``headers`` : ``header`` array

``subject`` : ``string``

``body`` : ``part`` array

``attachments`` : ``part`` array

Parts that do not have a ``Content-Disposition`` of ``attachment``  are included in ``attachment``.

Otherwise, they are included in ``body`` (``multipart-related``, ``multipart/alternative``).

When creating a MIME, you can also specify a ``subject_charset`` and ``mime_type`` in ``string``.

When creating a MIME, the ``Date`` header is decided by ``utc_date``. If omitted, ``local_date`` is used instead.

When creating MIME, ``subject``, ``id``, ``mime_type`` take precedence over header definition. For example, you can specify the ``multipart/*`` type without specifying it in the header, or over-riding whatever is defined in the header.

---

* Properties of an ``address`` object

``name`` : ``string``

``string`` : ``string``

``encoded_string`` : ``string``

``addr`` : ``string``

``idn_addr`` : ``string``

When creating a MIME, the only properties needed are ``name`` and ``addr``.

---

* Properties of a ``header`` object

``name`` : ``string``

``value`` : ``string``

When creating a multi-part MIME, make sure you specifiy the ``Content-Type`` header in the ``message`` object or use the ``mime_type`` property, especially when you have multiple parts. If there is only 1 ``body``, then the ``Content-Type`` of that part becomes the ``Content-Type`` of the ``message`` object. If there are 2 or more ``body`` elements, the ``Content-Type`` or ``mime_type`` of the ``message`` object (``multipart/alternative``, ``multipart/mixed``, ``multipart/related``, etc) has an impact on how the message is rendered by a client.

---

* Properties of a ``part`` object

``mime_type`` : ``string``

``media_type`` : ``string``

``media_subtype`` : ``string``

``file_name`` : ``string``

``data`` : ``string`` for ``text/*``, ``number`` for other types 

``content_description`` : ``string`` (optional)

``content_md5`` : ``string`` (optional)

``content_location`` : ``string`` (optional)

``content_encoding`` : ``string``

``headers`` : ``header`` array

When creating a MIME, make sure you specifiy the ``content_encoding`` in ``part``, not just as a header value.

When creating a MIME, you can also specify a ``charset`` as ``string``.

When creating MIME, ``mime_type``, ``content_id``, ``content_location``, ``content_description``, ``file_name``, ``content_md5`` take precedence over header values. ``content_encoding`` is always base64 for binary parts.

## Example of parsing MIME

```
$path:=Get 4D folder(Current resources folder)+"test.eml"

DOCUMENT TO BLOB($path;$eml)

C_TEXT($mime_json)
ARRAY BLOB($mime_blobs;0)
MIME PARSE MESSAGE ($eml;$mime_json;$mime_blobs)

$mime:=JSON Parse($mime_json)

ARRAY OBJECT($body;0)
OB GET ARRAY(OB Get($mime;"message";Is object);"body";$body)

$subject:=ob get (OB Get($mime;"message";Is object);"subject";Is text)

$plain:=OB Get($body{1};"data";Is text)
$html:=OB Get($body{2};"data";Is text)
```

## Example of creating MIME

```
$path:=Get 4D folder(Current resources folder)+"test.eml"

DOCUMENT TO BLOB($path;$eml)

C_TEXT($mime_json)
ARRAY BLOB($mime_blobs;0)
MIME PARSE MESSAGE ($eml;$mime_json;$mime_blobs)

$preview:=JSON Parse($mime_json)

//add an attachment

$path:=Get 4D folder(Current resources folder)+"wakanda-mascot-walking.png"
DOCUMENT TO BLOB($path;$image)
APPEND TO ARRAY($mime_blobs;$image)
$message:=OB Get($preview;"message")
ARRAY OBJECT($body;0)
OB GET ARRAY($message;"body";$body)
C_OBJECT($b)
OB SET($b;\
  "file_name";"わかんだちゃん.png";\
  "content_description";"わかんだちゃん";\
  "content_location";"http://www.4d.com";\
  "content_md5";Generate digest("わかんだちゃん";MD5 digest);\
  "mime_type";"image/png";\
  "data";1)  //element number of this blob in $2
ARRAY OBJECT($headers;0)
C_OBJECT($h)
OB SET($h;"name";"Content-Disposition";"value";"inline")
APPEND TO ARRAY($headers;$h)
OB SET ARRAY($b;"headers";$headers)
APPEND TO ARRAY($body;$b)
OB SET ARRAY($message;"body";$body)
OB SET($message;"mime_type";"multipart/mixed")

$mime_json:=JSON Stringify($preview)

$eml:=MIME Create message ($mime_json;$mime_blobs)

$path:=System folder(Desktop)+"1.eml"
BLOB TO DOCUMENT($path;$eml)
```
