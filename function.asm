; Function that returns 7 segment display from certain digit

			.cdecls C,LIST,"msp430.h"       ; Include device header file
			.def	initLEDs
            .text
initLEDs	bis.b   #BIT3,&P4DIR            ; set P4.3 as out
			bis.b   #BIT4,&P4DIR            ; set P4.4 as out
			bis.b   #BIT5,&P4DIR            ; set P4.5 as out
			bis.b   #BIT6,&P4DIR            ; set P4.6 as out
			
			bic.b	#BIT3,&P4OUT            ; clear P4.3
			bic.b	#BIT4,&P4OUT            ; clear P4.4
			bic.b	#BIT5,&P4OUT            ; clear P4.5
			bic.b	#BIT6,&P4OUT            ; clear P4.6
			ret

			.end
