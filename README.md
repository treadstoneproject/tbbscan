Table of content
============
* [Parallel Scanning Core Engine.](#parallel-scanning)
* [File types support scanning.](#file-types-support)
* [Signature support scanning.](#signature-support)

-------------------------------------------------------------------------- 

<a name="parallel-scanning">Parallel Scanning Core Engine.</a>

* Signature Engine use Thread Building Block(TBB) for applying concurrent insert bytes of signature from malware database.
* Scanning Engine run with Multithread  and Parallel_for  checks bytes of binary stream on vector concurrent.

<a name="file-types-support">File types support scanning.</a>

* Programs read data from struct name "meta_sig" contains member variable name "std::string file_name".  File_name variable call to path contains binary file.
* Hex characters of binary byte streams. 

<a name="signature-support">Signature support scanning.</a>

* MD5, SHA-1, SHA-256 and hex byte of instruction from reverse engineering technique.
