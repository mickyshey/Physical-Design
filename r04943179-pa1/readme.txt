1. 學號： r04943179
2. 姓名： 謝佳霖
3. 使用之語言： < C++ >
4. 使用之編譯器： < GNU g++ >
5. 檔案壓縮方式： < tar zcvf r04943179-p1.tgz r04943179-p1 >
6. 各檔案說明：
		r04943179-p1/Makefile 				Makefile to compile
		r404943179-p1/FM					executable binary
		r04943179-p1/main.cpp				source code
		r04943179-p1/NetCell.cpp			source code
		r04943179-p1/NetCell.h				source code
		r04943179-p1/FM_partition.cpp		source code
		r04943179-p1/FM_partition.h			source code
		r04943179-p1/report.doc				report on the data structures used in the program
		r04943179-p1/input_pa1				sample inputs 

7. 編譯方式說明：
	主程式： 請在主程式的目錄下，鍵入make指令，即可完成編譯，
			 在主程式的目錄下會產生一個名為 FM 的執行檔
			 若要重新編譯，請先執行 make clean 再執行一次 make

8. 執行、使用方式說明：
	主程式： 編譯完成之後，在檔案目錄下會產生一個 FM 的執行檔
			 執行檔的命令格式為：
			 ./FM <input file name> <output file name> 
			 例如： 要對 input_1.dat 做 FM_partition 且在 input_1.out 中寫出結果
			 則在命令提示下鍵入
			 ./FM input_1.dat input_1.out

9. 執行結果說明：
	主程式： 主程式在執行的時候會顯示目前已執行的 iteration 數及 cutsize
			 執行完則會在目標目錄中找到產生的 output file

<註>
   若產生編譯錯誤，而將在edaunion再次編譯，請在edaunion U5 上面進行，因其他server可能有不支援c++11的狀況。
