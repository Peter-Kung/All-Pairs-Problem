 # Using MPI to Parallelize an All-Pairs Problem
 
 ## 1. The Problem Description
 
  You are given a table T consisting of two attributes, x and y. The table is stored as n tuples of x and y in a file. The values inside a tuple are separated by a tab character; each tuple ends with a new line character (therefore a tuple is a line).

Based on a given file, you are asked to write an MPI program to calculate the distance of each pair of (x,y), and then find the top 5 shortest distances and their corresponding pairs.
 
 ![image](https://user-images.githubusercontent.com/30169115/176693018-f7990c9b-8fe4-4ad1-bd58-2601cc57a1cc.png)

 
## 2. Explain how to compile and run the program

- How to compile:

```c
$ make
```

- How to run:

```c
$ make qtest
```

- Play with different core quntity

```c
CORE_N := 2(the core number you wnat to run)
```

- Play with different data scale
  - modify `data number` in the `main.c` program
  - modify main.c `freope`n path to `test.txt`
  - modify `input_gem.c`’s `data number`: `n`
  - >$ make thtest

- Monitor time cost
  - default is open, you can close it by cancel macro `TIMETEST`

## 3. Describes how you parallelize the computations
 
1. 傳遞 n 數量的點座標
2. 之後算 n / (worker process 數量) 的點與其他所有點的距離
3. 回傳距離最短的五個
4. 找出所有回傳最短距離的五個
 
![image](https://user-images.githubusercontent.com/30169115/176693593-ab5ed11a-4957-4b73-bf2f-8116ec2b39ed.png)

 
## 4. Scalability

10000 data scale, 100 times statistics:

![image](https://user-images.githubusercontent.com/30169115/176693760-f9302eba-6b8e-427a-97b7-ee0efcb417a5.png)

 
## 5. Test Run Result
 
  Result after Run part one example: 
 
 ![image](https://user-images.githubusercontent.com/30169115/176694058-503152ce-1e14-49da-9eb4-a74e4c8bf324.png)

 
## 6. Reference
[Find execution time of a C program](https://www.techiedelight.com/find-execution-time-c-program/)

[Plotting multiple data files](https://riptutorial.com/gnuplot/example/27408/plotting-multiple-data-files)

[Gnuplot FAQ](http://www.gnuplot.info/faq/index.html#x1-190002.5)

[Gnuplot](https://hackmd.io/@jasonmatobo/Linux_Kernel_Note_2021/%2F%40jasonmatobo%2Fgnuplot)

