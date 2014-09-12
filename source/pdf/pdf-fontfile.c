#define STB_TRUETYPE_IMPLEMENTATION
#include "mupdf/pdf.h"
#include "stb_truetype.h"
#include "dirent.h"

/*
	Which fonts are embedded is based on a few preprocessor definitions.

	The base 14 fonts are always embedded.
	For font substitution we embed DroidSans which has good glyph coverage.
	For CJK font substitution we embed DroidSansFallback.

	Set NOCJK to skip all CJK support (this also omits embedding the CJK CMaps)
	Set NOCJKFONT to skip the embedded CJK font.
	Set NOCJKFULL to embed a smaller CJK font without CJK Extension A support.

	Set NODROIDFONT to use the base 14 fonts as substitute fonts.
*/

#ifdef NOCJK
#define NOCJKFONT
#endif

#include "gen_font_base14.h"

#ifndef NODROIDFONT
#include "gen_font_droid.h"
#endif

#ifndef NOCJKFONT
#ifndef NOCJKFULL
#include "gen_font_cjk_full.h"
#else
#include "gen_font_cjk.h"
#endif
#endif

char * fontlink = NULL;

unsigned char *
pdf_lookup_builtin_font(const char *name, unsigned int *len)
{
	if (!strcmp("Courier", name)) {
		*len = sizeof pdf_font_NimbusMon_Reg;
		return (unsigned char*) pdf_font_NimbusMon_Reg;
	}
	if (!strcmp("Courier-Bold", name)) {
		*len = sizeof pdf_font_NimbusMon_Bol;
		return (unsigned char*) pdf_font_NimbusMon_Bol;
	}
	if (!strcmp("Courier-Oblique", name)) {
		*len = sizeof pdf_font_NimbusMon_Obl;
		return (unsigned char*) pdf_font_NimbusMon_Obl;
	}
	if (!strcmp("Courier-BoldOblique", name)) {
		*len = sizeof pdf_font_NimbusMon_BolObl;
		return (unsigned char*) pdf_font_NimbusMon_BolObl;
	}
	if (!strcmp("Helvetica", name)) {
		*len = sizeof pdf_font_NimbusSan_Reg;
		return (unsigned char*) pdf_font_NimbusSan_Reg;
	}
	if (!strcmp("Helvetica-Bold", name)) {
		*len = sizeof pdf_font_NimbusSan_Bol;
		return (unsigned char*) pdf_font_NimbusSan_Bol;
	}
	if (!strcmp("Helvetica-Oblique", name)) {
		*len = sizeof pdf_font_NimbusSan_Ita;
		return (unsigned char*) pdf_font_NimbusSan_Ita;
	}
	if (!strcmp("Helvetica-BoldOblique", name)) {
		*len = sizeof pdf_font_NimbusSan_BolIta;
		return (unsigned char*) pdf_font_NimbusSan_BolIta;
	}
	if (!strcmp("Times-Roman", name)) {
		*len = sizeof pdf_font_NimbusRom_Reg;
		return (unsigned char*) pdf_font_NimbusRom_Reg;
	}
	if (!strcmp("Times-Bold", name)) {
		*len = sizeof pdf_font_NimbusRom_Med;
		return (unsigned char*) pdf_font_NimbusRom_Med;
	}
	if (!strcmp("Times-Italic", name)) {
		*len = sizeof pdf_font_NimbusRom_Ita;
		return (unsigned char*) pdf_font_NimbusRom_Ita;
	}
	if (!strcmp("Times-BoldItalic", name)) {
		*len = sizeof pdf_font_NimbusRom_MedIta;
		return (unsigned char*) pdf_font_NimbusRom_MedIta;
	}
	if (!strcmp("Symbol", name)) {
		*len = sizeof pdf_font_StandardSymL;
		return (unsigned char*) pdf_font_StandardSymL;
	}
	if (!strcmp("ZapfDingbats", name)) {
		*len = sizeof pdf_font_Dingbats;
		return (unsigned char*) pdf_font_Dingbats;
	}
	*len = 0;
	return NULL;
}

unsigned char *
pdf_lookup_substitute_font(char *fontname, int mono, int serif, int bold, int italic, unsigned int *len)
{
#ifdef NODROIDFONT
	if (mono) {
		if (bold) {
			if (italic) return pdf_lookup_builtin_font("Courier-BoldOblique", len);
			else return pdf_lookup_builtin_font("Courier-Bold", len);
		} else {
			if (italic) return pdf_lookup_builtin_font("Courier-Oblique", len);
			else return pdf_lookup_builtin_font("Courier", len);
		}
	} else if (serif) {
		if (bold) {
			if (italic) return pdf_lookup_builtin_font("Times-BoldItalic", len);
			else return pdf_lookup_builtin_font("Times-Bold", len);
		} else {
			if (italic) return pdf_lookup_builtin_font("Times-Italic", len);
			else return pdf_lookup_builtin_font("Times-Roman", len);
		}
	} else {
		if (bold) {
			if (italic) return pdf_lookup_builtin_font("Helvetica-BoldOblique", len);
			else return pdf_lookup_builtin_font("Helvetica-Bold", len);
		} else {
			if (italic) return pdf_lookup_builtin_font("Helvetica-Oblique", len);
			else return pdf_lookup_builtin_font("Helvetica", len);
		}
	}
#else
	if(fontsfolder != NULL) //fontsfolder extern variable : "mupdf/pdf.h"
		fontlink = getFonts(fontname);

	if(fontlink == NULL){
		if (mono) {
			*len = sizeof pdf_font_DroidSansMono;
			return (unsigned char*) pdf_font_DroidSansMono;
		} else {
			*len = sizeof(pdf_font_DroidSans);
			return (unsigned char*)pdf_font_DroidSans;
		}
	}else{	
		*len = read_ttf_file_size(fontlink);
		return (unsigned char*)read_ttf_file(fontlink);
	}
#endif
}

char* getFonts(char *fontname)
{
	int nbronfonts = NbrOfFonts(fontsfolder);
	int p0 = 0;
	char* fontnameCopie;
	DIR *dir;
	struct dirent *ent;

if(mapremplie!=1)
{
	tablefontmap =  malloc(nbronfonts * sizeof(fontmap)); //tablefontmap extern variable : "mupdf/pdf.h"
	if ((dir = opendir(fontsfolder)) != NULL) {
	    while ((ent = readdir (dir)) != NULL) {
		if((int)ent->d_type == 8){
		    char * fontFile = (char *) malloc(1 + strlen(fontsfolder)+ strlen(ent->d_name) );
		    strcpy(fontFile,fontsfolder);
		    strcat(fontFile,ent->d_name);
		    stbtt_fontinfo font;
		    int size = read_ttf_file_size(fontFile);

		    unsigned char *mybuffer = malloc(size + 1);
		    fread(mybuffer, 1, size, fopen(fontFile, "r"));
		    stbtt_InitFont(&font, mybuffer, 0);
		    
		    int platformID = 1;
		    int encodingID = 0;
		    int languageID = 0;
		    int nameID = 6 ;  

		    stbtt_int32 i,count,stringOffset;
		    stbtt_uint8 *fc = font.data;
		    stbtt_uint32 offset = font.fontstart;
		    stbtt_uint32 nm = stbtt__find_table(fc, offset, "name");

		    if (nm){
		        count = ttUSHORT(fc+nm+2);
		        stringOffset = nm + ttUSHORT(fc+nm+4);
		        for (i=0; i < count; ++i) {
		            stbtt_uint32 loc = nm + 6 + 12 * i;
		            if (platformID == ttUSHORT(fc+loc) && encodingID == ttUSHORT(fc+loc+2)&& languageID == ttUSHORT(fc+loc+4) && nameID == ttUSHORT(fc+loc+6)) {
		                const char *fontTXT = (const char *) (fc+stringOffset+ttUSHORT(fc+loc+10));
		                char name[ttUSHORT(fc+loc+8)+1];
		                strncpy(name,fontTXT,ttUSHORT(fc+loc+8));
		                name[ttUSHORT(fc+loc+8)] ='\0';
				fontmap* az = (fontmap*)malloc(sizeof(fontmap));
				az->fontfile=fontFile;
				fontnameCopie = (char*)malloc(strlen(name) * sizeof(char*));
				strcpy(fontnameCopie,name);
				az->fontname=fontnameCopie;
				tablefontmap[p0]=(fontmap*)az;
				p0++;

		            }
		        }
		    }
		}
	  }
	  closedir (dir);
	}
	int k ;
	for(k=0;k<nbronfonts;k++)
	{
		if((strstr(tablefontmap[k]->fontname,fontname)) != NULL)
		{
			return (char * )tablefontmap[k]->fontfile;
		}else if((strstr(fontname,tablefontmap[k]->fontname)) != NULL)
			{
				return (char * )tablefontmap[k]->fontfile;
			}
	}
	mapremplie=1;
}else{
	int k ;
	for(k=0;k<nbronfonts;k++)
	{
		if((strstr(tablefontmap[k]->fontname,fontname)) != NULL){
			return (char * )tablefontmap[k]->fontfile;
		}else if((strstr(fontname,tablefontmap[k]->fontname)) != NULL){
			return (char * )tablefontmap[k]->fontfile;}
	}
}

return NULL;
}

unsigned char *read_ttf_file(char *fontFile)
{	
	long int size = read_ttf_file_size(fontFile);
	FILE *file = fopen(fontFile, "r");	
	unsigned char * in = (unsigned char *) malloc(size);
	fread(in, sizeof(unsigned char), size, file);
	fclose(file);
return (unsigned char *) in;
}

int read_ttf_file_size(char *fontname)
{
	FILE * file = fopen(fontname, "r");
	if (file == NULL)
		return 0;
	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fclose(file);
return size;
}

int NbrOfFonts(const char* Dir)
{
	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	if((dirp = opendir(Dir))!=NULL){; /* There should be error handling after this */
	while ((entry = readdir(dirp)) != NULL) {
	    if (entry->d_type == DT_REG) { /* If the entry is a regular file */
		 file_count++;
	    }
	}
	closedir(dirp);	}
	return file_count;
}

unsigned char *
pdf_lookup_substitute_cjk_font(int ros, int serif, int wmode, unsigned int *len, int *index)
{
#ifndef NOCJKFONT
#ifndef NOCJKFULL
	*index = wmode;
	*len = sizeof pdf_font_DroidSansFallbackFull;
	return (unsigned char*) pdf_font_DroidSansFallbackFull;
#else
	*index = wmode;
	*len = sizeof pdf_font_DroidSansFallback;
	return (unsigned char*) pdf_font_DroidSansFallback;
#endif
#else
	*len = 0;
	return NULL;
#endif
}
