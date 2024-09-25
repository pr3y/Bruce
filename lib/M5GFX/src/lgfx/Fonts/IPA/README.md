# lgfx_font_japan

https://moji.or.jp/ipafont/

IPAexフォントおよびIPAフォントをu8g2形式に変換したファイルです。

## TTF to BDF

otf2bdf 3.1(http://sofia.nmsu.edu/~mleisher/Software/otf2bdf/)

```
./otf2bdf -r 72 -p  8 -o lgfx_font_japan_mincho_8.bdf    ipam.ttf
./otf2bdf -r 72 -p 12 -o lgfx_font_japan_mincho_12.bdf   ipam.ttf
./otf2bdf -r 72 -p 16 -o lgfx_font_japan_mincho_16.bdf   ipam.ttf
./otf2bdf -r 72 -p 20 -o lgfx_font_japan_mincho_20.bdf   ipam.ttf
./otf2bdf -r 72 -p 24 -o lgfx_font_japan_mincho_24.bdf   ipam.ttf
./otf2bdf -r 72 -p 28 -o lgfx_font_japan_mincho_28.bdf   ipam.ttf
./otf2bdf -r 72 -p 32 -o lgfx_font_japan_mincho_32.bdf   ipam.ttf
./otf2bdf -r 72 -p 36 -o lgfx_font_japan_mincho_36.bdf   ipam.ttf
./otf2bdf -r 72 -p 40 -o lgfx_font_japan_mincho_40.bdf   ipam.ttf
./otf2bdf -r 72 -p  8 -o lgfx_font_japan_mincho_p_8.bdf  ipaexm.ttf
./otf2bdf -r 72 -p 12 -o lgfx_font_japan_mincho_p_12.bdf ipaexm.ttf
./otf2bdf -r 72 -p 16 -o lgfx_font_japan_mincho_p_16.bdf ipaexm.ttf
./otf2bdf -r 72 -p 20 -o lgfx_font_japan_mincho_p_20.bdf ipaexm.ttf
./otf2bdf -r 72 -p 24 -o lgfx_font_japan_mincho_p_24.bdf ipaexm.ttf
./otf2bdf -r 72 -p 28 -o lgfx_font_japan_mincho_p_28.bdf ipaexm.ttf
./otf2bdf -r 72 -p 32 -o lgfx_font_japan_mincho_p_32.bdf ipaexm.ttf
./otf2bdf -r 72 -p 36 -o lgfx_font_japan_mincho_p_36.bdf ipaexm.ttf
./otf2bdf -r 72 -p 40 -o lgfx_font_japan_mincho_p_40.bdf ipaexm.ttf
./otf2bdf -r 72 -p  8 -o lgfx_font_japan_gothic_8.bdf    ipag.ttf
./otf2bdf -r 72 -p 12 -o lgfx_font_japan_gothic_12.bdf   ipag.ttf
./otf2bdf -r 72 -p 16 -o lgfx_font_japan_gothic_16.bdf   ipag.ttf
./otf2bdf -r 72 -p 20 -o lgfx_font_japan_gothic_20.bdf   ipag.ttf
./otf2bdf -r 72 -p 24 -o lgfx_font_japan_gothic_24.bdf   ipag.ttf
./otf2bdf -r 72 -p 28 -o lgfx_font_japan_gothic_28.bdf   ipag.ttf
./otf2bdf -r 72 -p 32 -o lgfx_font_japan_gothic_32.bdf   ipag.ttf
./otf2bdf -r 72 -p 36 -o lgfx_font_japan_gothic_36.bdf   ipag.ttf
./otf2bdf -r 72 -p 40 -o lgfx_font_japan_gothic_40.bdf   ipag.ttf
./otf2bdf -r 72 -p  8 -o lgfx_font_japan_gothic_p_8.bdf  ipaexg.ttf
./otf2bdf -r 72 -p 12 -o lgfx_font_japan_gothic_p_12.bdf ipaexg.ttf
./otf2bdf -r 72 -p 16 -o lgfx_font_japan_gothic_p_16.bdf ipaexg.ttf
./otf2bdf -r 72 -p 20 -o lgfx_font_japan_gothic_p_20.bdf ipaexg.ttf
./otf2bdf -r 72 -p 24 -o lgfx_font_japan_gothic_p_24.bdf ipaexg.ttf
./otf2bdf -r 72 -p 28 -o lgfx_font_japan_gothic_p_28.bdf ipaexg.ttf
./otf2bdf -r 72 -p 32 -o lgfx_font_japan_gothic_p_32.bdf ipaexg.ttf
./otf2bdf -r 72 -p 36 -o lgfx_font_japan_gothic_p_36.bdf ipaexg.ttf
./otf2bdf -r 72 -p 40 -o lgfx_font_japan_gothic_p_40.bdf ipaexg.ttf
```

## BDF to c

https://github.com/olikraus/u8g2/blob/master/tools/font/bdfconv/bdfconv.exe
https://github.com/olikraus/u8g2/blob/master/tools/font/build/japanese3.map

上記のbdfconv.exeと、japanese3.mapを元に、文字を追加したja.mapを利用しています。

```
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_8.bdf    -o ..\output\lgfx_font_japan_mincho_8.c    -n lgfx_font_japan_mincho_8
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_12.bdf   -o ..\output\lgfx_font_japan_mincho_12.c   -n lgfx_font_japan_mincho_12
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_16.bdf   -o ..\output\lgfx_font_japan_mincho_16.c   -n lgfx_font_japan_mincho_16
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_20.bdf   -o ..\output\lgfx_font_japan_mincho_20.c   -n lgfx_font_japan_mincho_20
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_24.bdf   -o ..\output\lgfx_font_japan_mincho_24.c   -n lgfx_font_japan_mincho_24
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_28.bdf   -o ..\output\lgfx_font_japan_mincho_28.c   -n lgfx_font_japan_mincho_28
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_32.bdf   -o ..\output\lgfx_font_japan_mincho_32.c   -n lgfx_font_japan_mincho_32
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_36.bdf   -o ..\output\lgfx_font_japan_mincho_36.c   -n lgfx_font_japan_mincho_36
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_40.bdf   -o ..\output\lgfx_font_japan_mincho_40.c   -n lgfx_font_japan_mincho_40
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_8.bdf  -o ..\output\lgfx_font_japan_mincho_p_8.c  -n lgfx_font_japan_mincho_p_8
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_12.bdf -o ..\output\lgfx_font_japan_mincho_p_12.c -n lgfx_font_japan_mincho_p_12
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_16.bdf -o ..\output\lgfx_font_japan_mincho_p_16.c -n lgfx_font_japan_mincho_p_16
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_20.bdf -o ..\output\lgfx_font_japan_mincho_p_20.c -n lgfx_font_japan_mincho_p_20
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_24.bdf -o ..\output\lgfx_font_japan_mincho_p_24.c -n lgfx_font_japan_mincho_p_24
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_28.bdf -o ..\output\lgfx_font_japan_mincho_p_28.c -n lgfx_font_japan_mincho_p_28
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_32.bdf -o ..\output\lgfx_font_japan_mincho_p_32.c -n lgfx_font_japan_mincho_p_32
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_36.bdf -o ..\output\lgfx_font_japan_mincho_p_36.c -n lgfx_font_japan_mincho_p_36
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_mincho_p_40.bdf -o ..\output\lgfx_font_japan_mincho_p_40.c -n lgfx_font_japan_mincho_p_40
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_8.bdf    -o ..\output\lgfx_font_japan_gothic_8.c    -n lgfx_font_japan_gothic_8
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_12.bdf   -o ..\output\lgfx_font_japan_gothic_12.c   -n lgfx_font_japan_gothic_12
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_16.bdf   -o ..\output\lgfx_font_japan_gothic_16.c   -n lgfx_font_japan_gothic_16
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_20.bdf   -o ..\output\lgfx_font_japan_gothic_20.c   -n lgfx_font_japan_gothic_20
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_24.bdf   -o ..\output\lgfx_font_japan_gothic_24.c   -n lgfx_font_japan_gothic_24
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_28.bdf   -o ..\output\lgfx_font_japan_gothic_28.c   -n lgfx_font_japan_gothic_28
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_32.bdf   -o ..\output\lgfx_font_japan_gothic_32.c   -n lgfx_font_japan_gothic_32
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_36.bdf   -o ..\output\lgfx_font_japan_gothic_36.c   -n lgfx_font_japan_gothic_36
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_40.bdf   -o ..\output\lgfx_font_japan_gothic_40.c   -n lgfx_font_japan_gothic_40
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_8.bdf  -o ..\output\lgfx_font_japan_gothic_p_8.c  -n lgfx_font_japan_gothic_p_8
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_12.bdf -o ..\output\lgfx_font_japan_gothic_p_12.c -n lgfx_font_japan_gothic_p_12
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_16.bdf -o ..\output\lgfx_font_japan_gothic_p_16.c -n lgfx_font_japan_gothic_p_16
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_20.bdf -o ..\output\lgfx_font_japan_gothic_p_20.c -n lgfx_font_japan_gothic_p_20
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_24.bdf -o ..\output\lgfx_font_japan_gothic_p_24.c -n lgfx_font_japan_gothic_p_24
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_28.bdf -o ..\output\lgfx_font_japan_gothic_p_28.c -n lgfx_font_japan_gothic_p_28
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_32.bdf -o ..\output\lgfx_font_japan_gothic_p_32.c -n lgfx_font_japan_gothic_p_32
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_36.bdf -o ..\output\lgfx_font_japan_gothic_p_36.c -n lgfx_font_japan_gothic_p_36
bdfconv.exe -v -b 0 -f 1 -M "japanese3.map" ..\bdf\lgfx_font_japan_gothic_p_40.bdf -o ..\output\lgfx_font_japan_gothic_p_40.c -n lgfx_font_japan_gothic_p_40
copy ..\output\*.c lgfx_font_japan.c
```

上記のファイルから /U8G2_FONT_SECTION\(".*"\) // の置換をしています。

