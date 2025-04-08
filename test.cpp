#include<cassert>
#include<cstring>

// #define assert_static(e) \
//     do{ \
//         enum {assert_static__=1/(e)}; \
//     } while (0)
    

// template<typename T,typename U>
// int bit_copy(T& a,U& b)
// {
//     static_assert(sizeof(b)==sizeof(a),"a==b");
//     memcpy(&a,&b,sizeof(b));
// }

// int main()
// {
//     int a=0x2468;
//     double b;
//     bit_copy(a,b);
// }

// template<class T>
// void fun() noexcept(noexcept(T())) {}

// void* operator new[] (std::size_t) noexcept(false);

#include<iostream>
// struct A
// {
//     ~A(){throw 1;}
// };
// struct B
// {
//     ~B() noexcept(false){throw 1;}
// };
// struct C
// {
//     B b;
// };

// void funa(){A a;}
// void funb(){B b;}
// void func(){C c;}
// int main()
// {
//     try
//     {
//         funb();
//     }
//     catch(...)
//     {
//         std::cout<<"caught funb"<<std::endl;
//     }
//     try
//     {
//         func();
//     }
//     catch(...)
//     {
//         std::cout<<"caught func"<<std::endl;
//     }
//     try
//     {
//         funa();
//     }
//     catch(...)
//     {
//         std::cout<<"caught funa"<<std::endl;
//     }
//     return 0;
// }
// class b
// {
// public:
//     b(int i):m(i){}
//     int get()
//     {
//         return m;
//     }
// private:
//     int m;
// };
// class a
// {
// public:
//     a(){}
//     a(int i):data(i){}
//     void get()
//     {
//         std::cout<<data<<" "<<m.get()<<" "<<name<<std::endl;
//     }
// private:
//     int data{1};
//     b m=0;
//     std::string name="group";
// };
// int main()
// {
//     a a;
//     a.get();
//     return 0;
// }

// struct a
// {
//     a(int a=3,double b=2.4){std::cout<<"hello"<<a<<b<<"\n";}
//     a(const a& obj){std::cout<<"no\n";}
// };
// struct b:a
// {
//     using a::a;
// };
// int main()
// {
//     b q1;
//     b q2(4);
//     b q3(7.4);
//     b q4=q1;
//     b q5(q2);
//     a q6;
//     a q7(q6);
//     return 0;
// }

// struct A{A(int i){std::cout<<"hello"<<i<<"\n";}};
// struct B{B(int i){std::cout<<"hello"<<i<<"\n";}};

// struct C:A,B
// {
//     // using A::A;
//     // using B::B;
//     C(int i):A(i),B(i){}
// };

// int main()
// {
//     C c1(1);
//     return 0;
// }

// class info1
// {
// public:
//     info1(){init();}
//     info1(int i):info1()
//     {
//         type=i;
//     }
//     info1(char e):info1(){name=e;}
// private:
//     void init()
//     {
//         type+=1;
//     }
//     int type{1};
//     char name{'a'};
// };
// class info2
// {
// public:
//     info2():info2(1,'a'){}
//     info2(int i):info2(i,'a'){}
//     info2(char e):info2(1,e){}
// private:
//     info2(int i,char e):type(i),name(e)
//     {
//         type+=1;
//     }
//     int type;
//     char name;
// };

// int main()
// {
//     info2 f(3);
//     return 0;
// }

#include<list>
#include<vector>
#include<deque>
class TD
{
    template<typename T> 
        TD(T first,T last):l(first,last){}
    std::list<int> l;
public:
    TD(std::vector<short>& v):TD(v.begin(),v.end()){}
    TD(std::deque<int>& d):TD(d.begin(),d.end()){}
    void show()
    {
        for(auto& kv:l)
        {
            std::cout<<kv<<"\n";
        }
    }
};
int main()
{
    std::vector<short> b{1,2,3,4,5};
    TD a1(b);
    a1.show();
    return 0;
}