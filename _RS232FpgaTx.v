module RS232FpgaTx //Отправка данных с ПЛИС
(
input clk,
input [7:0] SW, // байт данных
input SW10, //при включении данные передаются, когда выключено- режим ожидания 

output reg tx //линия передачи
);

parameter idle = 2'b00; //режим ожидания
parameter startbit = 2'b01; //стартбит
parameter databit = 2'b10; //режим передачи данных
parameter stopbit = 2'b11; //стопбит

//делитель счётчика: 50Мгц(импульсов/с)/9600(бит/с)=5208(импульсов/с)
//обеспечивает нужную скорость передачи бит
parameter divider = 13'd5208; 

reg [3:0] state, nextstate; // текущее и следующее состояние автомата
reg [12:0] tickcount; // счётчик импульсов
integer i;

initial
begin
	state = idle; //текущее состояние - ожидание
	nextstate = idle; //следующее состояние - ожидание
	i=0;
	tickcount = 0; // счётчик импульсов обнулён
end



always @(posedge clk)
begin
state = nextstate;

case (state) 
	
	idle: 
	begin 
		tickcount = tickcount + 1; 
		if (tickcount == divider)
		begin
			tickcount = 0;
			tx=1'b1;
			if (SW10==1'b1)
				nextstate = startbit;
			else
				nextstate = state;			
		end
		else nextstate = state;
	end
		
	startbit:
	begin
		tickcount = tickcount + 1; 
		if (tickcount == divider)
		begin
			tickcount = 0;
			tx=1'b0;	
			nextstate = databit;
		end
		else nextstate = state;
	end
		
	databit:
	begin
		if (i!=8)
		begin
			tickcount = tickcount + 1;
			if (tickcount == divider)
			begin
				tickcount = 0;
				tx = SW[i];
				i = i + 1;
			end
			else nextstate = state;
		end
		else nextstate = stopbit; 	
	end	
			
			
	stopbit:
	begin
		tickcount = tickcount + 1; 
		if (tickcount == divider)
		begin
			tickcount = 0;
			tx=1'b1;	
			nextstate = idle;
		end
		else nextstate = state;	
	end
	

	default: nextstate = idle;
	
endcase;	
			
end



endmodule