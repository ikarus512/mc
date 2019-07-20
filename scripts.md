# Useful scripts

## common
for f in $(ls *.jpg);do echo $f; done
n=5; for i in $(seq 1 $n);do echo $i; done

## Image (imagemagic)
### image resize
for f in $(ls jpg0);do convert jpg0/$f -geometry 1600x jpg1/$f; done
### image concat
convert U01.png U02.png -append U1.png  # join vertically
convert U03.png U04.png -append U2.png  # join vertically
convert U1.png  U2.png  +append U.png   # join horizontally
### image conversion
dst=2; if [ -e $dst ];then echo $dst folder already exists; exit; fi; mkdir $dst
n=$(ls *.jpg | wc -w)
i=0; for f in $(ls *.jpg);do i=$(($i+1)); echo $i/$n $f;
  convert $f -resize x700 -strip -quality 70% $dst/$f
  echo "  <p> <ru></ru><en></en> <br> <img src=$f> <br>" >>$dst/index.htm
done

## Audio
### Audio conversion
ffmpeg -i input.wav -vn -ar 44100 -ac 2 -b:a 64k -f mp3 output.mp3

## Video
### Video conversion, with audio copied without conversion
quality=3; size=720x540; fs="00000 00001"; for f in $fs;do ffmpeg -i mov0/$f.MTS -q $quality -r 25 -s $size -b 300 -vcodec mpeg4 -c:a copy mov1/$f.avi; done
### Video conversion to ogv(ogg) format (install libtheora,libvorbis, and ffmpeg should be configured as "./configure --enable-libtheora --enable-libvorbis"):
for f in $(ls $src);do ffmpeg -i $src/$f -codec:v libtheora -qscale:v 7 -s 360x270 -codec:a libvorbis -qscale:a 5  $dst/$f.ogg; done
### Video conversion to old hardware DVD player format:</en><ru></ru><pre>ffmpeg -i in.avi -vcodec mpeg2 -acodec libmp3lame -target dvd out.avi -y</pre>

## PDF
### Compress
ps2pdf in.pdf out.pdf
### pdftk
  pdftk input.pdf burst                             ### Break input.pdf into one-page files
  pdftk input.pdf cat 1-endE output output.pdf      ### Rotate pages 1 to end to East direction (do this after crop!)
  pdftk 1.pdf 2.pdf 3.pdf output output.pdf         ### Merge several files to one pdf
  gs -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dFirstPage=1 -dLastPage=47 -sOutputFile=out.pdf in.pdf ### Merge2 (not remove unvisible page sides)
  convert -compress JPEG -quality 80 in.pdf out.pdf ### Compress
  pdftk i.pdf output o.pdf uncompress; convert -compress Zip in.pdf out.pdf ### Compress 2
  #Crop, format A4 portrait is 595x842points=210x297mm (72points=1inch=25.4mm, 28.3p=10mm):
  left=28; right=28; top=28; bottom=28; width=595; height=842; r=$(expr $width - $right); t=$(expr $height - $top) #A4=595x842p portrait
  gs -o output.pdf -sDEVICE=pdfwrite -c [/CropBox [$left $bottom $r $t] /PAGES pdfmark -f input.pdf
  #Crop1:
  gs -o o.pdf -sDEVICE=pdfwrite -c "[/CropBox [24 72 559 794] /PAGES pdfmark" -f i.pdf
  gs -o o.pdf -sDEVICE=pdfwrite -dDEVICEWIDTHPOINTS=595 -dDEVICEHEIGHTPOINTS=842 -dFIXEDMEDIA -c "24 72 translate 0 0 535 722 rectclip" -f i.pdf
  gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dPDFSETTINGS=/screen -dNOPAUSE -dQUIET -dBATCH -sOutputFile=o.pdf i.pdf
  gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4                       -dNOPAUSE -dQUIET -dBATCH -sOutputFile=o.pdf i.pdf
  gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dDownsampleColorImages=true -dColorImageResolution=150 -dNOPAUSE  -dBATCH -sOutputFile=output.pdf input.pdf
### DJVU to PDF conversion (dependecies: djvulibre, pdftk):
f=1.djvu; n=$(djvudump $f | grep -Po "\d+ pages" | grep -Po "\d+"); fs=; for i in $(seq 1 $n);do ddjvu -format=pdf -page=$i $f $f-page-$i.pdf; fs="$fs $f-page-$i.pdf"; done; pdftk $fs output 1.pdf
