proc Tree: var = 31 mod 15 = 1
sig Tree: var = 31 mod 13 = 5

procTree:
	var1: 
	1 -> (2, 3, 4)	2 -> (5, 6)	6 -> 7	7 -> 8


sigTree:
	var5:

	1->(2,3) SIGUSR1   3->4 SIGUSR2   4->(5,6,7) SIGUSR1   7->8 SIGUSR1   8->1 SIGUSR2

					1
		        +---------------|---------------+
			|		|		|
		+-------2-------+	3		4
		|		|
		5		6	
				|
				7
				|
				8

===================================================================================================
				SU1		SU1
			+---------------1---------------+
			|				|
			2				3
							| SU2
							4
							|SU1
						+-------|-------+
						|	|	|
						5	6	7
								| SU1
								8
								| SU2
								1
					
	

1. Для каждого процесса свой обработчик сигнала, в котором процесс принимает от предыдущего и посылает следующему по таблице
2. Контроль ошибок -> stderr по шаблону: 'unit_name: message'  <===>  Error open file: 1.txt
3. Имя модуля и имя файла берутся из командной строки
4. Каждый процусс при посылке/получении сигнала выводит:
	N  pid  ppid  receive/send  USR1/USR2  time(mс-sec)
    N -- номер сына по таблице

5. После 101-го сигнала SIGUSR 1 процесс посылает сыновьям SIGTERM и ожидает их завершения


