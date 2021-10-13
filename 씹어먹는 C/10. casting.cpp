#include <stdio.h>

int main(){
	//소수점 아래 2자리 출력하는 프로그램 
	float f;
	scanf("%f", &f);
	int i=(int)(f*100)%100;//% can operate only int type. 
	i=i>0? i:-i;//- to +
	printf("%2d\n",i);//%2 for distinguish 0.09, 0.9
	
	return 0;
} 

/*
허허 행복하구려 https://www.youtube.com/watch?v=obkZl2mYnpI
1.	int에 double같은 실수 데이터를 넣었을때, 소숫점이 잘리는 이유는 컴퓨터가 실수를 표현하는 방식에 있다.
	컴퓨터상에서 실수는 고정 소수점(Fixed Point)과 부동 소수점(Floating Point)으로 존재한다. 다만 부동 소수점이 메모리상 더 효율적이기에 대부분 컴퓨터는 부동 소수점 방식을 사용한다.(다만 정밀도는 떨어짐) 
	  소수는 10^-2 처럼 표시할 수 있는데, 컴퓨터상에서도 마찬가지로 +-fxb^e(가수, 밑, 지수)꼴로 표현한다. (IEEE 754표준) 
	10010.1011(2)=2^4+2^1+2^-1+2^-1+2^-4=18+0.5+0.125+0.0625=18.6875 처럼 나타낸다. 
2.	안타깝게도 모든 10진법의 수들은 2진법으로의 변환이 불가능한데, 대표적으로 이진화시 무한소수가 나타나게되는 0.1이 그렇다. 
	고로 메모리에 일정부분만 잘라서 보관하게 되기에 필연적으로 오차가 발생한다.
3.	IEEE 754방식 하에서 소수가 저장되는 메카니즘은 아래와 같다.
	부호비트틑 설정하고, 변환된 이진수를 1.xxxx꼴로 정규화한다. 이 정규화 작업에서 shift연산의 횟수가 지수로 들어가게 된다.
	0.1과 같이 무한소수로 표현되는 수들의 경우 type의 크기에 맞추어 반올림을 한다. 그 후 지수에 2^(e-1)-1을 더해주는 바이어스(Bias)처리를 해준다(이때 e의 값은 지수부분의 비트수이다.)
	 Bias처리의 이유는 지수는 언제나 양수여야 하는데, shift연산의 방향에 따라 음수가 되었을 수도 있기 때문이다. 
	Bias처리 없이 2의 보수 표현법으로 지수를 나타낼 수도 있지만, 컴퓨터 입장에서는 무조건 양수로 값을 넣는 것이 크기 비교에 수월하다.
4.	8비트로 처리할 수 있는 수의 범위는 0~255이지만, float은 1~254까지의 지수만을 처리한다. 그 이유는 IEE 754에서 정상적이지 않은 수를 표현하기 위해 비트를 사용하기 때문이다.
	정상적이지 않은 수로는 비정상 수(Benormalized number)_너무 작아서 Bias처리를 해도 1이상이 안되는 수들_inf, 무한대, 수가 아님(NaN)_Not-a-Number like inf/inf이 있다. 
5.	캐스팅을 할 시에는 명시적으로 표시해주는 것이 좋은데, 타 프로그래머 입장에서 실수인지 고의인지 판단이 어렵기 때문이다. 
*/
