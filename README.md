![version](https://img.shields.io/badge/version-19%2B-5682DF)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-gmime)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-gmime/total)

# 4d-plugin-gmime

The MIME plugin parses raw email/MIME source (an `.eml`-style byte stream) into a structured `Object` plus a set of `Blob`s for binary payloads, and does the reverse: builds a raw MIME message from an `Object` description plus a set of `Blob`s. Internally it's built on GMime; results are handed back to 4D as plain `Text` (JSON) and `Blob` values, so nothing GMime-specific ever reaches your 4D code.

## Summary

| Command | Returns | Purpose |
|---|---|---|
| [MIME PARSE MESSAGE](#mime-parse-message) | (none — fills parameters 2 and 3) | Parse a raw email/MIME `Blob` into a JSON description (`Text`) and an array of binary payload `Blob`s |
| [MIME Create message](#mime-create-message) | `Blob` | Build a raw email/MIME message from a JSON description (`Text`) and an array of binary payload `Blob`s |

**Platforms:** Windows, macOS (both commands are declared `threadSafe` in the plugin manifest and can be called from preemptive processes)

---

## Requirements & platform notes

- Both commands take their full parameter list unconditionally — there's no optional/short form of either syntax.
- **`MIME PARSE MESSAGE` never raises a 4D error.** Malformed or unparseable input results in empty/default output values (see [Error handling](#error-handling--troubleshooting)), not an exception you can catch in 4D.
- **`MIME Create message` returns an empty (zero-length) `Blob` on any failure** — malformed JSON, a JSON root without a `message` object, or an internal error while building the message. This graceful-empty-return behavior is part of a fix applied during this plugin's most recent review; it guarantees the command always returns something rather than potentially leaving the call hanging. If you're on an older build of this plugin, verify that behavior before relying on it.
- Date fields exchanged with the plugin use ISO 8601 with a numeric UTC offset (`%Y-%m-%dT%H:%M:%S%z`, e.g. `2024-03-01T09:30:00+0100`) — that's the exact format both commands read and write.
- Text body encoding defaults to UTF-8 unless you specify otherwise (see [MIME Create message](#mime-create-message) below).
- Binary content between 4D and the plugin never travels inside the JSON itself — the JSON only carries an index number, and the actual bytes live in the accompanying `Array Blob` parameter. Keep the JSON and the blob array together; they're only meaningful as a pair.

---

## MIME PARSE MESSAGE

### Syntax

```4d
MIME PARSE MESSAGE ( message ; result ; parts )
```

| Parameter | Type | Description |
|---|---|---|
| `message` | Blob | The raw email/MIME source to parse (the exact bytes of an `.eml` file, or whatever your SMTP/IMAP/POP3 layer handed you) |
| `result` | Text | Filled with a JSON-formatted description of the message (see [Result object](#result-object) below) |
| `parts` | Array Blob | Filled with the raw binary payload of every part/attachment that carries binary data; individual entries are referenced from `result` by index |
| Function result | — | None — this command has no return value; it's a procedure that fills `result` and `parts` |

### Description

`message` must contain a complete, unmodified raw email source — headers and body, as you'd get from reading an `.eml` file into a `Blob` with `DOCUMENT TO BLOB`, or from a mail-protocol library that returns raw source. The plugin parses it in RFC-loose compliance mode (relaxed address, parameter, and RFC 2047 encoded-word compliance), so it will do a best-effort parse of malformed real-world mail rather than rejecting it outright.

`result` is always filled with a JSON object of the shape `{"message": {...}}`. Every key described below lives inside that inner `message` object.

#### Result object

| Property | Type | Description |
|---|---|---|
| `from`, `cc`, `to`, `bcc`, `sender`, `reply_to`, `all_recipients` | Array (of address objects) | Always present, always an array (empty `[]` if the header was absent). See [Address object](#address-object). |
| `id` | Text | The `Message-ID` header value. **Omitted entirely** if the message had no `Message-ID`. |
| `subject` | Text | The decoded `Subject` header. **Omitted entirely** if absent. |
| `local_date` | Text or Null | The message date, in the sender's local time zone, as ISO 8601. `Null` if the `Date` header was missing or unparseable. |
| `local_time` | Integer or Null | The local time-of-day component of `local_date`, in milliseconds since midnight. `Null` under the same condition as `local_date`. |
| `utc_date` | Text or Null | The message date converted to UTC, as ISO 8601. |
| `utc_time` | Integer or Null | The UTC time-of-day component, in milliseconds since midnight. |
| `headers` | Array (of `{name, value}`) | The message's raw header list. **Omitted entirely** if, for any reason, the message ends up with zero headers (see the caveat below). |
| `body` | Array (of [part objects](#part-object)) | The message's text/inline body part(s). **Omitted entirely** if none were found — check for the key's existence before iterating, don't assume an empty array. |
| `attachments` | Array (of [part objects](#part-object)) | Top-level attachments (anything not classified as body). Same omit-if-empty rule as `body`. |
| `parts` | Array (of [part objects](#part-object)) | Only appears for messages containing a `message/rfc822` attachment (a forwarded email) whose *own* internal sub-parts aren't body-eligible. This is a nesting detail you'll rarely need; see the forwarded-message note below. |

#### Address object

Each entry in `from`/`cc`/`to`/etc.:

| Property | Type | Description |
|---|---|---|
| `string` | Text | The address formatted as `"Display Name" <addr@example.com>`, unencoded |
| `encoded_string` | Text | Same, with RFC 2047/2231 encoding applied where needed (safe to drop straight back into a raw header) |
| `addr` | Text | The bare email address |
| `idn_addr` | Text | The address with any internationalized domain converted to its ASCII (Punycode) form |
| `name` | Text | The display name only, undecorated |

#### Part object

Each entry in `body`/`attachments`/`parts`:

| Property | Type | Description |
|---|---|---|
| `headers` | Array (of `{name, value}`) | This part's own headers |
| `data` | Text or Integer | **Text** for a text part with a resolvable charset — already-decoded content, ready to use. **Integer** for binary content — a 1-based index into the `parts` blob-array parameter; look up `parts{data}` (4D arrays are 1-based) for the raw bytes. |
| `mime_type` | Text | Full content type, e.g. `text/plain` |
| `media_type` / `media_subtype` | Text | The same content type split into its two components, e.g. `text` / `plain` |
| `content_id` | Text | Present only if the part had a `Content-ID` |
| `content_encoding` | Text | The original `Content-Transfer-Encoding` (e.g. `base64`, `quoted-printable`), present only if set |
| `file_name` | Text | Resolved from the filename, the `Content-Type` `name` parameter, or the `Content-Disposition` `filename` parameter, in that fallback order; present only if any of the three yielded a name |
| `content_description` | Text | The `Content-Description` header, if present |
| `content_md5` | Text | The `Content-MD5` header, if present |
| `content_disposition` | Text | `inline`, `attachment`, etc., if present |

An entry that is itself a forwarded `message/rfc822` attachment additionally carries the **same** `from`/`cc`/`to`/`bcc`/`sender`/`reply_to`/`all_recipients`/`id`/`subject`/`local_date`/`local_time`/`utc_date`/`utc_time` properties described for the top-level `message` object above — i.e. it looks like a nested message object. In that case `data` (an index into `parts`) is the complete raw source of the forwarded message — headers and body — so you can hand that same blob straight back into another call of `MIME PARSE MESSAGE` if you want to parse the forwarded message recursively.

**Caveat — parts with zero headers are silently dropped.** If a MIME part somehow has no headers at all (extremely rare — a well-formed part always has at least `Content-Type`), the plugin skips it entirely rather than including a bare/partial entry. This is a genuine edge of the current implementation, not a documented "feature" — don't rely on every physical part in the source always appearing in the result.

### Example

```4d
var $emlBlob : Blob
var $json : Text
var $parts : Object
ARRAY BLOB($parts;0)

DOCUMENT TO BLOB("/path/to/message.eml";$emlBlob)

MIME PARSE MESSAGE($emlBlob;$json;$parts)

var $message : Object
$message:=JSON Parse($json).message

ALERT("Subject: "+String($message.subject))

If($message.body#Null)
	var $bodyPart : Object
	For each($bodyPart;$message.body)
		If(Value type($bodyPart.data)=Is text)
			TRACE  // $bodyPart.data is decoded text, ready to display
		Else
			var $bytes : Blob
			$bytes:=$parts{$bodyPart.data}
		End if
	End for each
End if
```

A second example, walking attachments and saving each one to disk:

```4d
If($message.attachments#Null)
	var $att : Object
	For each($att;$message.attachments)
		If(Value type($att.data)=Is real)
			var $fileName : Text
			$fileName:=(($att.file_name#Null) ? $att.file_name : "attachment.bin")
			BLOB TO DOCUMENT("/path/to/output/"+$fileName;$parts{$att.data})
		End if
	End for each
End if
```

---

## MIME Create message

### Syntax

```4d
$emlBlob:=MIME Create message ( description ; parts )
```

| Parameter | Type | Description |
|---|---|---|
| `description` | Text | JSON description of the message to build (see [Description object](#description-object) below) |
| `parts` | Array Blob | Binary payloads referenced by index from `description`'s part objects |
| Result | Blob | The fully-built raw MIME message (CRLF line endings), ready to save as `.eml` or hand to an SMTP send routine |

### Description

`description` must be JSON of the shape `{"message": {...}}` — same envelope shape `MIME PARSE MESSAGE` returns, though the two aren't perfectly symmetric (see the notes below). If the JSON doesn't parse, or the root has no `message` object, the command returns an empty (zero-length) `Blob`.

#### Description object

| Property | Type | Description |
|---|---|---|
| `from`, `cc`, `to`, `bcc`, `sender`, `reply_to` | Array (of `{name, addr}`) | Only `name` and `addr` are read from each entry — if you're round-tripping a `MIME PARSE MESSAGE` result, the extra `string`/`encoded_string`/`idn_addr` keys are simply ignored. **`all_recipients` is not consumed at all** — it's a read-only, derived field on the parse side. |
| `subject` | Text | The subject line |
| `subject_charset` | Text | Optional; if provided, the subject is set with this explicit charset. If omitted, no charset override is passed (the library's default applies). |
| `id` | Text | Optional; sets the `Message-ID` header |
| `mime_type` | Text | Optional; overrides the message's own top-level `Content-Type`. You typically don't need this — the plugin derives the right multipart structure automatically from `body`/`attachments` (see below). |
| `headers` | Array (of `{name, value, charset}`) | Optional; sets arbitrary extra headers. **`from`, `cc`, `to`, `bcc`, `sender`, `reply_to`, and `subject` are silently ignored if you put them in this array** — those must go through their dedicated keys above instead. |
| `utc_date` or `local_date` | Text (ISO 8601) | Optional; sets the `Date` header. `utc_date` is tried first; `local_date` is used only if `utc_date` is absent or fails to parse. |
| `body` | Array (of [part objects](#part-object-1)) | The message's body part(s) |
| `attachments` | Array (of [part objects](#part-object-1)) | The message's attachment part(s) |

#### Part object

Each entry in `body`/`attachments`:

| Property | Type | Description |
|---|---|---|
| `data` | Text or Integer | **Text** — literal content for a text part (the plugin creates a text part and encodes it itself). **Integer** — a 1-based index into the `parts` blob-array parameter, for binary content. |
| `mime_type` | Text | Sets this part's `Content-Type`, e.g. `text/html`, `image/png` |
| `charset` | Text | Only used when `data` is a literal string; defaults to `utf-8` if omitted |
| `content_encoding` | Text | Explicit `Content-Transfer-Encoding` — **only honored when `data` is a literal string.** For blob-referenced (`data` is a number) parts, the encoding is always forced to `base64` regardless of what you put here. |
| `content_id` | Text | Optional; sets `Content-ID` |
| `content_location` | Text | Optional; sets `Content-Location` |
| `content_description` | Text | Optional; sets `Content-Description` |
| `file_name` | Text | Optional; sets the attachment's filename |
| `content_md5` | Text | Optional; sets `Content-MD5` |
| `headers` | Array (of `{name, value, charset}`) | Optional; extra headers on this specific part |

**Content-Transfer-Encoding defaults to `base64` in both cases** — for blob-referenced data unconditionally, and for literal text data whenever you don't set `content_encoding` yourself. If you want a plain-text body sent as `7bit`/`quoted-printable` instead of `base64`, you must set `content_encoding` explicitly.

**Multipart structure is automatic.** You don't build the `multipart/alternative`, `multipart/related`, or `multipart/mixed` wrapper structure yourself — the plugin picks it based on how many `body` and `attachments` entries you supply (e.g. two text `body` entries become `multipart/alternative`; a text body plus an inline image becomes `multipart/related`; any attachments alongside a body become `multipart/mixed`). Just supply flat `body`/`attachments` arrays.

### Example

```4d
var $desc : Object
$desc:=New object
$desc.message:=New object

$desc.message.subject:="Hello from 4D"
$desc.message.from:=New collection(New object("name";"4D App";"addr";"app@example.com"))
$desc.message.to:=New collection(New object("name";"Jane Doe";"addr";"jane@example.com"))

$desc.message.body:=New collection(\
	New object("mime_type";"text/plain";"data";"Plain text version.");\
	New object("mime_type";"text/html";"data";"<p>HTML version.</p>"))

var $parts : Object
ARRAY BLOB($parts;0)

var $emlBlob : Blob
$emlBlob:=MIME Create message(JSON Stringify($desc);$parts)

BLOB TO DOCUMENT("/path/to/output/message.eml";$emlBlob)
```

A second example, attaching a file read from disk via the blob-index mechanism:

```4d
var $fileBlob : Blob
DOCUMENT TO BLOB("/path/to/report.pdf";$fileBlob)

ARRAY BLOB($parts;1)
$parts{1}:=$fileBlob

$desc.message.attachments:=New collection(\
	New object("mime_type";"application/pdf";"file_name";"report.pdf";"data";1))

$emlBlob:=MIME Create message(JSON Stringify($desc);$parts)
```

---

## Error handling & troubleshooting

- **`MIME PARSE MESSAGE` never raises a catchable 4D error.** A malformed or truncated `message` blob results in a best-effort parse (GMime's loose-compliance parsing) rather than a hard failure; check the returned `result` JSON for missing/empty fields instead of expecting an exception.
- **`MIME Create message` returns a zero-length `Blob` rather than raising an error** on malformed JSON, a missing `message` object, or an internal failure while building the message — check `Blob size($emlBlob)=0` after the call if you need to detect failure. (This graceful-failure behavior was added in a recent fix to this plugin; older builds may behave differently on the same bad input.)
- **`body`/`attachments`/`parts` keys can be absent, not just empty**, in a `MIME PARSE MESSAGE` result — always check `$message.body#Null` (or equivalent) before iterating; don't assume an empty collection.
- **A MIME part with zero headers is silently dropped** from the parsed result rather than appearing as a partial entry — if a part count in your result looks lower than expected, this is the most likely reason for genuinely malformed source.
- **`from`/`cc`/`to`/etc. in a `MIME Create message` description only read `name` and `addr`** from each address entry — passing through the full address object from a `MIME PARSE MESSAGE` result (with `string`/`encoded_string`/`idn_addr` included) is harmless; those extra keys are just ignored.
- **You cannot override `From`/`To`/`Cc`/`Bcc`/`Sender`/`Reply-To`/`Subject` via the generic `headers` array** when building a message — entries with those names are silently skipped. Use the dedicated top-level keys instead.
- **Binary content never appears inline in the JSON.** A numeric `data` value on either side of the API is always an index into the accompanying `Array Blob` parameter (1-based) — if you're building or reading the JSON by hand, don't expect to find raw bytes there.
- **Text vs. binary parts default to different encodings only if you let them** — everything defaults to `base64` transfer encoding on the create side unless you explicitly set `content_encoding` on a literal-text part.

---

## Quick reference

```4d
// Parse
var $emlBlob : Blob
var $json : Text
var $parts : Object
ARRAY BLOB($parts;0)
DOCUMENT TO BLOB($path;$emlBlob)
MIME PARSE MESSAGE($emlBlob;$json;$parts)
var $message : Object
$message:=JSON Parse($json).message

// Create
var $desc : Object
$desc:=New object("message";New object(\
	"subject";"Hi";\
	"from";New collection(New object("name";"Me";"addr";"me@example.com"));\
	"to";New collection(New object("name";"You";"addr";"you@example.com"));\
	"body";New collection(New object("mime_type";"text/plain";"data";"Hello."))))
ARRAY BLOB($createParts;0)
var $out : Blob
$out:=MIME Create message(JSON Stringify($desc);$createParts)
BLOB TO DOCUMENT($outPath;$out)
```
