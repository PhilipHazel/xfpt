# Makefile for making xfpt documentation

# In Arch Linux, need to install the "xmlto" and "docbook-xsl" packages in 
# order to make the html version. From time to time it may be necessary to
# edit MyStyle-nochunk-html.xsl if the version number of xsl-stylesheets
# changes.

all:       xfpt.pdf xfpt.html

xfpt.xml:  xfpt.xfpt
	   xfpt xfpt.xfpt

xfpt.ps:   xfpt.xml
	   sdop xfpt.xml

xfpt.pdf:  xfpt.ps
	   ps2pdf xfpt.ps xfpt.pdf

xfpt.html: xfpt.xml
	   /bin/rm -rf xfpt.html
	   xmlto -x MyStyle-nochunk-html.xsl html-nochunks xfpt.xml
	   ./TidyHTML
           
clean:;    /bin/rm -f *.html *.pdf *.ps *.xml
 
# End
