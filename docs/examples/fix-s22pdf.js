// A simple script to fix the broken fonts in PDF files generated by S22PDF.

if (scriptArgs.length != 2) {
	print("usage: mutool run fix-s22pdf.js input.pdf output.pdf");
	quit();
}

var doc = Document.openDocument(scriptArgs[0]);

var font = new Font("zh-Hans");
var song = doc.addCJKFont(font, "zh-Hans", "H", "serif");
var heiti = doc.addCJKFont(font, "zh-Hans", "H", "sans-serif");
song.Encoding = 'GBK-EUC-H';
heiti.Encoding = 'GBK-EUC-H';

var MAP = {
	"/#CB#CE#CC#E5": song, // SimSun
	"/#BA#DA#CC#E5": heiti, // SimHei
	"/#BF#AC#CC#E5_GB2312": song, // SimKai
	"/#B7#C2#CB#CE_GB2312": heiti, // SimFang
	"/#C1#A5#CA#E9": song, // SimLi
}

var i, n = doc.countPages();
for (i = 0; i < n; ++i) {
	var fonts = doc.findPage(i).Resources.Font;
	if (fonts) {
		fonts.forEach(function (font, name) {
			if (font.BaseFont in MAP && font.Encoding == 'WinAnsiEncoding')
				fonts[name] = MAP[font.BaseFont];
		});
	}
}

doc.save(scriptArgs[1]);
