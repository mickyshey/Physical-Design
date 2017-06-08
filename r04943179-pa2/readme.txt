1. 學號： r04943179
2. 姓名： 謝佳霖
3. 使用之語言： < C++ >
4. 使用之編譯器： < GNU g++ >
5. 檔案壓縮方式： < tar zcvf r04943179-pa2.tgz r04943179-pa2 >
6. 各檔案說明：
		r04943179-pa2/src/Makefile 				Makefile to compile
		r04943179-pa2/src/floorplan				executable binary
		r04943179-pa2/src/main.cpp				source code
		r04943179-pa2/src/FloorplanMgr.cpp		source code
		r04943179-pa2/src/FloorplanMgr.h		source code
		r04943179-pa2/src/Block.cpp				source code
		r04943179-pa2/src/Block.h				source code
		r04943179-pa2/src/Util.cpp				source code
		r04943179-pa2/src/Util.h				source code
		r04943179-pa2/report.doc				report on the data structures used in the program
		r04943179-pa2/readme.txt
		r04943179-pa2/input_pa2					sample inputs 

7. 編譯方式說明：
	主程式： 請在src目錄下，鍵入make指令，即可完成編譯，
			 在src目錄下會產生一個名為 floorplan 的執行檔
			 若要重新編譯，請先執行 make clean 再執行一次 make

8. 執行、使用方式說明：
	主程式： 編譯完成之後，在src目錄下會產生一個 floorplan 的執行檔
			 執行檔的命令格式為：
			 ./floorplan <alpha value> <input_block file name> <input_net file name> <output file name>
			 例如： 要在 alpha = 0.5 下對 input1.block 和 input1.net 做 floorplanning 且在 input1.out 中寫出結果
			 則在命令提示下鍵入
			 ./floorplan 0.5 input1.block input1.net input1.out

9. 執行結果說明：
	主程式： 主程式在執行的時候會先random construct 100 個 BTree ，並計算average area/wire length，並輸出：

			 avgArea: <area>				// computed average area
			 avgWireLength: <length>		// computed average wire length
			 #illegal: <#>					// number of unfit BTree in random construction

			 接著進入 SA ，過程中會顯示目前的 runtime 、溫度及 current best cost:

			 time: <time> 		, T: <T>		, current cost: <cost>

			 執行完會輸出得到最好的BTree資訊：

			 time: <time>					// runtime of SA
			 legal: <bool>					// if BTree is fit
			 area: <area>					// area of BTree
			 wire length: <length>			// wire length of BTree
			 cost: <cost>					// real cost of BTree

			 最後會在目標目錄中找到產生的 output file

<註>
   若產生編譯錯誤，而將在edaunion再次編譯，請在edaunion U5 上面進行，因其他server可能有不支援c++11的狀況。
