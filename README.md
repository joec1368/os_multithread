# OS_Homework2
- [name=F74091093陳宥橋]
## 運行概念：
1. 在程式最一開始時，會先讀完兩個目標的 matrix，然後會判斷是第一個 matrix 的 row 比較長，還是第二個 matrix 的 col 比較長
2. 然後會以長得那邊為基準，做分割的動作。如果兩邊等長，就會以第一個 matrix 的 row 為基準，去做切割
3. 當開始切割後，每個 thread 會要做的數目為： base_number / thread_number 。如果今天有除不盡的狀況，就會交給 thread 0 去做處理。
    ```
    舉例 1 ：
        如果今天是 2048 * 1024 vs 1024 * 1024
        thread number = 3
        那就會以第一個 matirx 的 row 為基準去切出需要的份數
        所以每個 thread 要做的 row 數是：682 * 1024 vs 1024 * 1024
        但是 thread 0 做得比別人多一些 ： 684 * 1024 vs 1024 * 1024
        
     舉例 2 ：
        如果今天是 1024 * 1024 vs 1024 * 2048
        thread number = 3
        那就會以第二個 matirx 的 col 為基準去切出需要的份數
        所以每個 thread 要做的 row 數是：1024 * 1024 vs 1024 * 682
        但是 thread 0 做得比別人多一些 ： 1024 * 1024 vs 1024 * 684
    ```
4. 這裡多提一點，在做 matrix 的乘法運算前，我有先把第二個 matrix 去做行列互換。會這樣做的原因為，在 C 裡面，會以 row 為主去做運算，如果我們今天第二個 matrix 沒有行列互換，他就會要一直去取 col 的值，而這樣會導致 cache miss 和需要花費大量時間，去處理 cache miss
5. 在這支程式裡，我在 thread 要寫入 thread_info前面，會加上 `pthread_mutex_lock(&lock1)` ，預防會有多個 thread 同時寫入，造成問題。而在寫完並關閉檔案後，就會解除 lock 。

## 成果圖：
* 以下圖的每一個 thread ，都是跑完30次去取平均後的結果：
* case1 : 
    * ![](https://i.imgur.com/7MtTVOf.jpg)
* case2 : 
    * ![](https://i.imgur.com/aAAHVMq.jpg)
* case3 : 
    * ![](https://i.imgur.com/fbEwQ90.jpg)
* case4 : 
    * ![](https://i.imgur.com/LcFymmk.jpg)
    
## 結果討論：
1. 由上幾張圖可知，在 threads 的數量小於 core 的數量時，當 threads 數量越靠近，會使得所需時間越少。
    * 推測原因為：因為有四個 core ，所以最多可以跑四個 thread。所以當 thread 的數量小於 core 的數量時，會有 core 沒有在做運算，所以效率肯定不會比四個 core 都在運行還好。
2. 當 threads 的數量大於 core 的數量時，所需的時間，會略大於 threads 的數量等於 core 的數量時所需的時間。
    * 推測原因為：因為只有四個 core 能跑，會有一些 thread 需要等待其他 thread 跑完，或著和其他thread 一起輪流跑，這樣就會導致效率比“ thread 數量和 core 數量一樣 ”的效率還低，因為在這種狀況下，大家不需要輪流跑，只要專注把眼前的 thread 跑完即可。
3. 其他發現：
    * 在測試之餘，有發現說，如果第二個 array 沒有做行列互換的話，因為cache miss 的緣故，會導致效率非常的差，效率大該是有做行列互換的 1/2
    * 在測試時，有發現，module 創出來的 proc file，應該是特殊的 file
        * 我們再加上了 sudo 和 force 的情況下都刪不掉，所以推測說，他並不是一般的 file
        * ![](https://i.imgur.com/WFvPnMV.png)
    * 利用 module 創出來的 proc file，是由 file_operations 來決定該 file 有什麼功能，如果在這個 file_operations 裡面沒有設定讀、寫等其他功能的話，在外面就不能對其做相對應的操作
        * 在一開始撰寫的時候，如果我們沒有設定 procfile_read，那我們在外面就無法對其做 cat 等功能
        * ![](https://i.imgur.com/apJ4jkE.png)

        










