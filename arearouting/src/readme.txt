1. 學號： r04943179
2. 姓名： 謝佳霖
3. 使用之語言： < C++ >
4. 使用之編譯器： < GNU g++ >
5. 檔案壓縮方式： < tar zcvf r04943179-pa4.tgz r04943179-pa4 >
6. 各檔案說明：
		r04943179-pa3/src/Makefile 				Makefile to compile
		r04943179-pa3/src/legalizer				executable binary
		r04943179-pa3/src/main.cpp					source code
		r04943179-pa3/src/Legalizer/				source code
		r04943179-pa3/src/Parser/					source code
		r04943179-pa3/src/PlaceCommon/			source code
		r04943179-pa3/src/Placement/				source code
		r04943179-pa3/report.doc					report on the data structures used in the program
		r04943179-pa3/readme.txt
		r04943179-pa3/benchmark/					sample inputs

7. 編譯方式說明：
	主程式： 請在 src 目錄下，鍵入 make 指令，即可完成編譯，
			 在src目錄下會產生一個名為 legalizer 的執行檔
			 若要重新編譯，請先執行 make clean 再執行一次 make

8. 執行、使用方式說明：
	主程式： 編譯完成之後，在 src 目錄下會產生一個 legalizer 的執行檔
			 執行檔的命令格式為：
			 ./legalizer -aux <input file name>
			 例如： 要對 input1.aux 做 global placement 和 legalizaition 
			 則在命令提示下鍵入
			 ./legalizer -aux input1.aux 

9. 執行結果說明：
	主程式： 
			 主程式會先輸出目前使用的 benchmark 及相關資訊，之後會執行 legalization 並在最後輸出 legalization 所花的時間：

			 Benchmark: 	< file name >				// benchmark file name 
			 Memory usage: < memory >					// memory info
			 Core region: 	< X, Y >						// core region of the benchmark
	
			 //////Legalization//////

			 Runtime of Legalization: < time >		// total runtime used by legalization

			 legalization success!


				
			 ///////////////////
			 Benchmark:		< file name >				// benchmark file name
	
			 最後會在 src 目錄下產生相關的 .pl 及 .plt 檔。

<註>
   若產生編譯錯誤，而將在edaunion再次編譯，請在edaunion U5 上面進行，因其他server可能有不支援c++11的狀況。
