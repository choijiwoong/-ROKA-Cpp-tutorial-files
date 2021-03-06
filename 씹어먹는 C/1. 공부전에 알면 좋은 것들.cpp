/*
1.	컴퓨터의 운영체제가 CPU에게 램에 위치해 있는 프로그램의 시작점을 알려주면 명령어를 읽어나가는데, 이 과정에서
	CPU는 현재 햄의 어디에서 명령어를 읽어야 할지 계속 알아야 한다. 이는 지금 읽어들일 명령어의 위치(instruction pointer)만을 보관하는
	RIP같은 특별한 레지스터를 이용한다.
2.	프로그램에서 사용하는 공간이 이미 사용되고 있는것인지 판단이 불가능하기 때문에, CPU가 참조하는 메모리 주소값_가상 메모리(virtual memory)
	와 실제 참조하게될 메모리의 주소값_물리 메모리(physical memory)를 특별한 1대1변환과정에 의하여 분리시킨다.
	 이러한 변환 과정을 페이징(paging)이라고 하고, 변환이 되는 최소의 메모리 단위를 페이지(page)라고 한다. 어떠한 변환을 수행할지 기록한 테이블을
	페이지 테이블(page table)이라고 하는데, 각 프로그램마다 하나씩 가지고 있다. 고로 같은 가상 메모리 주소를 갖지만, 다름 물리주소를 의미할 수 있다. 
3.	62비트 CPU의 경우 레지스터의 크기는 8바이트이다. 
*/
