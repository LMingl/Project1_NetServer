#include <stdio.h>
#include<string.h>
#include"zdb/zdb.h"
//#include"zdb/Connection.h"
//#include"zdb/URL.h"
//#include"zdb/Exception.h"
#include"zdb/SQLException.h"
//#include"database_process"

// gcc -I /路径       -lzdb
//gcc test_db.c -I /usr/local/include/zdb -o test_db -lzdb

void database_process(void* arg){
    char data[128];
    strcpy(data, (char*)arg);

    long int numb;               //编号
    char re_time[128];             //时间
    double latitude;        //经度
    double longitude;       //纬度
    int scale;              //缩放比

    char* p = data;
    char* pre = p;
    char temp[128];
    int len;
    int i;

    //解析numb
    pre = strchr(p, '=');
    pre++;
    p = pre;
    while(*p != '&')
        p++;
    len = p - pre;
    for(i = 0; i < len; i++){
        temp[i] = pre[i];
    }
    temp[i+1] = '\0';
    sscanf(temp, "%ld", &numb);   //字符型转长整性

    //解析re_time
    memset(temp, 0, 128);
    pre = strchr(p, '=');
    pre++;
    p = pre;
    while(*p != '&')
        p++;
    len = p - pre;
    for(i = 0; i < len; i++){
        temp[i] = pre[i];
    }
    temp[i+1] = '\0';
    strcpy(re_time, temp);

    //解析latitude
    memset(temp, 0, 128);
    pre = strchr(p, '=');
    pre++;
    p = pre;
    while(*p != '&')
        p++;
    len = p - pre;
    for(i = 0; i < len; i++)
        temp[i] = pre[i];
    temp[i+1] = '\0';
    sscanf(temp, "%lf", &latitude);

    //解析longitude
    memset(temp, 0, 128);
    pre = strchr(p, '=');
    pre++;
    p = pre;
    while(*p != '&')
        p++;
    len = p - pre;
    for(i = 0; i < len; i++)
        temp[i] = pre[i];
    temp[i+1] = '\0';
    sscanf(temp, "%lf", &longitude);

    //解析scale
    memset(temp, 0, 128);
    pre = strchr(p, '=');
    pre++;
    p = pre;
    while(*p != '\0')
        p++;
    len = p - pre;
    for(i = 0; i < len; i++)
        temp[i] = pre[i];
    temp[i+1] = '\0';
    sscanf(temp, "%d", &scale);

    //对于通过http协议传输的时间，空格会被‘+’或 %20 取代
    //所以需要对收到的字符串进行处理
    printf("%s\n", re_time);
    int x, y;
    for(x = 0, y = 0; re_time[y]; x++, y++){
        if(re_time[y] == '+')
            re_time[x] = ' ';
        else if(re_time[y] == '%'){
            if(!(re_time[y+1]&&re_time[y+2]))
                break;
            re_time[x] = ' ';
            y += 2;
        }
        else
            re_time[x] = re_time[y];
    }
    re_time[x] = '\0';
    printf("%s\n", re_time);


    

    //printf("%ld\n%s\n%lf\n%lf\n%d\n", numb, re_time, latitude, longitude, scale);
/*    
CREATE DATABASE IF NOT EXISTS tracks;
ALTER DATABASE tracks CHARACTER SET utf8;#改成utf8可以使用中文字符
USE tracks;
CREATE TABLE IF NOT EXISTS register_info(
	id BIGINT(11) PRIMARY KEY,#编号
	real_name VARCHAR(4) UNIQUE NOT NULL,#
	gender CHAR(1),
	address VARCHAR(20) NOT NULL
);
CREATE TABLE IF NOT EXISTS track(
	numb BIGINT(11)NOT NULL,
	re_time DATETIME NOT NULL,
	latitude DOUBLE NOT NULL,
	longitude DOUBLE NOT NULL,
	scale INT NOT NULL DEFAULT 5,
	CONSTRAINT fk_track_register FOREIGN KEY(numb) REFERENCES register_info(id) #外键约束
);

ALTER TABLE register_info CONVERT TO CHARACTER SET utf8;#使用中文字符
ALTER TABLE track CONVERT TO CHARACTER SET utf8;#使用中文字符
*/
    URL_T url = URL_new("mysql://localhost/tracks?user=root&password=19951212");
    ConnectionPool_T pool = ConnectionPool_new(url);
    ConnectionPool_start(pool);

    Connection_T con = ConnectionPool_getConnection(pool);

    //Connection_execute(con, "insert into register_info(id, real_name, gender, address)values(%ld, 'john', 'm', '天津')", numb);
    //结论：libzdb不支持中文编码 
    //不一定对,可能我字符集没有设置好
    
    //涉及另一个表，计划开发一个web页面可以添加信息到register_info表中
    TRY
    {

    ResultSet_T r = Connection_executeQuery(con, "select id  from register_info where id=%ld", numb);
    if(ResultSet_next(r))
        printf("is not null\n");
    else
        printf(" null\n");
  /*  
    PreparedStatement_T pe;
    pe = Connection_prepareStatement(con, "insert into track(numb, re_time, latitude, longitude,scale) values(?,?,?,?,?)");
    PreparedStatement_setLLong(pe, 1, numb);
    PreparedStatement_setString(pe, 2, re_time);
    PreparedStatement_setDouble(pe, 3, latitude);
    PreparedStatement_setDouble(pe, 4, longitude);
    PreparedStatement_setInt(pe, 5, scale);
    PreparedStatement_execute(pe);
    */
    }
    CATCH(SQLException)
    {

    }
    FINALLY
    {

    }


    Connection_close(con);
    ConnectionPool_free(&pool);
    URL_free(&url);
}

int main(void)
{
    char a[512] = "numb=19853649073time=1992-04-03%2012:00:00&latitude=116.725241&longitude=26.594446&scale=5";
    database_process(a);

    return 0;
}

