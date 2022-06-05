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
コマンドプロンプトでアプリケーションのあるフォルダに移動し、以下のコマンドで実行できる。  
<br>
`officehttppicker.exe (ターゲットファイル) (ホワイトリストファイル)`  
<br>
または
<br>
`officehttppicker.exe (ターゲットファイル)`  
（ホワイトリストファイルを省略した場合、同一フォルダの「whitelist.txt」を参照する。）  
<br>
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

<br>  
## 開発環境
Microsoft Visual Studio Community 2017 Version 15.9.4    Microsoft Visual C++ 2017
<br>
## 問題点・制限事項
* 本アプリケーションは、Microsoft Officeファイルの仕様書を読んで十分理解して作られたものではない。したがって、アプリケーションとしては想定通りの動作であっても、目的を十分達成できない可能性は残される。  
* zipの解凍処理はzlibを流用しているが、これによる解凍が不十分となるケースがあり得る。これは、zlib側の問題ではなく、作成者のzlibやzipファイル形式についての理解が不十分であることに起因する。
* インシデントの調査目的で作成したため、作成期間が短いなどの制約があり、デバッグが不十分である可能性がある。
* 一部処理については、開発を容易にするために省略された方法を用いている。これによる検知不備の可能性がある。
<br>
## 著作権・ライセンス
LICENSEファイルを参照（一般的なMITライセンス）。  
流用等に制限はありませんが、著作権を放棄したものではありません。  
なお、当然ながらzlibの部分の著作権に関してはJean-loup Gailly氏およびMark Adler氏に帰属します。  
<br>
