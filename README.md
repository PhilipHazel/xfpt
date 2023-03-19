# The xfpt plain text to XML processor

xfpt is a program that converts a marked up source document coded in UTF-8 into
an XML document. It was written with DocBook XML in mind, but can also be used
to create other forms of XML. Unlike some other processors, xfpt does not try
to produce XML from input that is also usable as a freestanding document. This
makes it less ambiguous for large and/or complicated documents. xfpt is aimed
at users who understand the XML that they are generating. It makes it easy to
include literal XML, either in blocks, or within paragraphs. xfpt restricts
itself to two special characters that trigger all its processing. You can read
the [xfpt manual](https://github.com/PhilipHazel/xfpt/blob/main/doc/xfpt.pdf) on
this GitHub site.
