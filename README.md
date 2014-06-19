AStartPathFind
==============
cocos2dx + c ++ 做的一个a星寻路算法win32
1.
 选择源点 绿色方块
 2.
 选择目标点 红色方块
 3.
 屏幕上方有3个button set wall，用来设置wall，重复点击已经设置好的墙会取消。
 4.
 点击 run 或者 step 
 run: 一步显示过程及结果，其中黄色的方块代表在closelist的坐标，绿色方框代表在openlist中
 step:是单步跟踪，点击一次执行一步，（一步含义从openlist中找到f值最小，加入到closelist，然后将八个方向的neighbour放入openlist）
 浅蓝色方框代表路径。
 5.
 点击restart按钮  重新开始寻路
 
 其中方块右上方的数值是G值，
 左上方的数值是H值
 左下角为F
 
 
