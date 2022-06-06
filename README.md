# OfficeHttpPicker Ver 0.1
Office 2007以降の .docx 等のファイルをZip解凍し、読めるテキスト内で「http」を含む文字列を抽出するプログラム。  
<br>
## 目　的
Officeドキュメントのうち、Office 2007以降の形式のファイルで、自動的に外部のURLを参照する機能を悪用する手法が見られる。  
Office 2007以降の形式のファイルはzip圧縮されているため、これを解凍し、「http」が含まれる文字列を抽出することで、不審なURLが含まれていないかチェックできるのではないかと考えた。  
また、「http」が含まれる文字列では、既知の安全なURLも含まれるため、編集可能なホワイトリストを利用してそれらを除去することで、可視性を高めることができると考えた。  
これらを実行できるVisual C++ベースのプログラムである。  
<br>
## 動作環境
Windows 10が動作するPCでの動作は確認済み。  
その他のバージョンは手元に無いため未確認だが、Win32でコマンドプロンプトが実行できるWindowsであれば概ね動くと考えられる。  
<br>
## 利用方法
公開しているアプリケーションは、コマンドプロンプト版のみとなっている。  
「Release」に格納されている「officehttppicker.exe」と「whitelist.txt」をダウンロードし、任意のフォルダに格納する。  
コマンドプロンプトでアプリケーションのあるフォルダに移動し、以下のコマンドで実行できる。  
<br>
`officehttppicker.exe (ターゲットファイル) (ホワイトリストファイル)`  
<br>
または  
<br>
`officehttppicker.exe (ターゲットファイル)`  
（ホワイトリストファイルを省略した場合、同一フォルダの「whitelist.txt」を参照する。）  
<br>
ホワイトリスト以外のhttpの文字列が見つかった場合、元のファイル名の拡張子を「.suspicious.txt」に変えたファイルを出力する。  
<br>
<br>
![image_01](https://github.com/Sachiel-archangel/OfficeHttpPicker/blob/main/image/image_01.png)
![image_02](https://github.com/Sachiel-archangel/OfficeHttpPicker/blob/main/image/image_02.png)
![image_03](https://github.com/Sachiel-archangel/OfficeHttpPicker/blob/main/image/image_03.png)
![image_04](https://github.com/Sachiel-archangel/OfficeHttpPicker/blob/main/image/image_04.png)

## 謝　辞
本アプリケーションでは、zipの解凍でJean-loup Gailly氏およびMark Adler氏によるzlib(zlib 1.2.12, March 27, 2022)を利用している。  
https://zlib.net/  
  
本アプリケーションのソースのうち、zlibによるものは以下のファイル。
* adler32.c
* crc32.c
* crc32.h
* gzguts.h
* inffast.c
* inffast.h
* inffixed.h
* inflate.c
* inflate.h
* inftrees.c
* inftrees.h
* zconf.h
* zlib.h
* zutil.c
* zutil.h

## 開発環境
Microsoft Visual Studio Community 2017 Version 15.9.4    Microsoft Visual C++ 2017
<br>
## 問題点・制限事項
* 本アプリケーションは、Microsoft Officeファイルの仕様書を読んで十分理解して作られたものではない。したがって、アプリケーションとしては想定通りの動作であっても、目的を十分達成できない可能性は残される。  
* zipの解凍処理はzlibを流用しているが、これによる解凍が不十分となるケースがあり得る。これは、zlib側の問題ではなく、作成者のzlibやzipファイル形式についての理解が不十分であることに起因する。
* インシデントの調査目的で作成したため、作成期間が短いなどの制約があり、デバッグが不十分である可能性がある。
* 一部処理については、開発を容易にするために省略された方法を用いている。これによる検知不備の可能性がある。
* メモリの使用効率があまり良くない（改善の余地あり）。
* エラー処理が甘く、またメッセージが不十分。
* Linux、Unix版のコードが無い。ただし、現状のコードでWindows依存なのはファイル操作、ファイルパス名操作、メモリの確保(VirtualAlloc)程度なので、そのあたりを乗せ換えれば対応できると思われる。この場合、文字コード(Unicode、UTF-8)対応に注意が必要。
* 今後のメンテナンスについては未定。

## 著作権・ライセンス  
LICENSEファイルを参照（一般的なMITライセンス）。  
流用等に制限はありませんが、著作権を放棄したものではありません。  
なお、当然ながらzlibの部分の著作権に関してはJean-loup Gailly氏およびMark Adler氏に帰属します。  
<br>
---
This is a program that Zip-decompresses files such as .docx of Office 2007 or later format and extracts character strings including "http" in readable text.  
## Purpose
Among Office documents, there is a method to exploit the function to automatically refer to an external URL in a file in Office 2007 or later format.  
I thought that since the files in Office 2007 and later formats are zipped, it might be possible to unzip them and extract the strings containing "http" to check for suspicious URLs.  
Also, since strings containing "http" also include known secure URLs, I thought that we could improve visibility by removing them using an editable whitelist.  
This is a Visual C ++ based application that can run these.  
<br>
## Operating environment
It has been confirmed to work on a PC running Windows 10.  
Other versions are unconfirmed because I do not have it at hand, but it seems that it will work on Windows that can execute the command prompt on Win32.  
<br>
## How to use
The published application is a command prompt version only.  
Download "officehttppicker.exe" and "whitelist.txt" stored in "Release" and store them in any folder.  
You can do this by navigating to the folder where the application is located at the command prompt and executing the following command:  
<br>
`officehttppicker.exe (target file) (whitelist file)`  
<br>
or  
<br>
`officehttppicker.exe (target file)`  
(If the whitelist file is omitted, "whitelist.txt" in the same folder is referenced.)  
<br>
If a string containing http that is not included in the white list is found, a file with the extension of the original filename changed to ".suspicious.txt" is output.  
The file will output a string containing the http that was not included in the white list.  
<br>
## Reference
This application uses zlib (zlib 1.2.12, March 27, 2022) by Jean-loup Gailly and Mark Adler for unzipping.    
https://zlib.net/  
  
The following file is the source of this application by zlib.  
* adler32.c
* crc32.c
* crc32.h
* gzguts.h
* inffast.c
* inffast.h
* inffixed.h
* inflate.c
* inflate.h
* inftrees.c
* inftrees.h
* zconf.h
* zlib.h
* zutil.c
* zutil.h

## Development environment
Microsoft Visual Studio Community 2017 Version 15.9.4    Microsoft Visual C++ 2017
<br>
## Problems and Limitations
* This application was not created with a full understanding of the Microsoft Office file specifications. Therefore, even if the application behaves as expected, the possibility remains that it may not fully achieve its objectives.  
* The zip decompression process uses zlib, but there can be cases where the decompression by this process is insufficient. This is not a problem with zlib, but with the creator's insufficient understanding of zlib and the zip file format.
* Since it was created for the purpose of investigating incidents, it may not have been debugged sufficiently due to limitations such as the short period of time in which it was created.
* Some processes use abbreviated methods to facilitate development. There is a possibility of inadequate detection due to this.  
* Memory usage is not very efficient.(That could be improved.)
* Error handling is lax and messages are inadequate.
* There is no code for Linux or Unix versions. However, the only Windows-dependent parts of the current code are file operations, file path name operations, and memory allocation (VirtualAlloc), so it should be possible to support Linux and Unix by replacing these parts. In this case, be careful about character encoding (Unicode, UTF-8) support.
* Future maintenance has not yet been determined.

## Copyrights and Licenses
Please see LICENSE file.  
There are no restrictions on appropriation, etc., but copyright is not waived.  
The zlib source is rightfully attributed to Jean-loup Gailly and Mark Adler.  
<br>
