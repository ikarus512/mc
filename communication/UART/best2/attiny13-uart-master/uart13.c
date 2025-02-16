#include "uart13.h"

/*
*   ISR(TIM0_COMPA_vect)
*   Прерывание таймера по сравнению с регистром OCR0A.  Оно работает постоянно и случается каждый раз, когда
*   таймер достигает значения, записанного в регистре OCR0A. Когда это происходит, значение таймера в регистре
*   TCNT0 автоматически обнуляется.
*   Это прерывание используется для отправки данных по UART. Переменная txbyte используется как сдвиговый регистр:
*   каждый раз, когда происходит прерывание, младший разряд переменной txbyte выставляется на вывод TXD микросхемы,
*   после чего происходит сдвиг содержимого переменной вправо, а в освободившийся старший разряд записывается
*   логическая единица. Пока данных для передачи нет, в переменной хранится число 0xFFFF и, тем самым, на выходе
*   TXD непрерывно поддерживается высокий логический уровень. Когда мы хотим передать данные, мы должны записать
*   в счетчик бит число бит для передачи: 1 стартовый, 8 бит данных и 1 стоповый, итого 10 (0x0A), и записать
*   в txbyte данные для передачи вместе со стартовым битом. После этого они немедленно начнут передаваться.
*   Формированием посылки занимается функция void uart_send(uint8_t tb).
*/

ISR(TIM0_COMPA_vect)
{
    TXPORT = (TXPORT & ~(1 << TXD)) | ((txbyte & 0x01) << TXD); // Выставляем в бит TXD младший бит txbyte
    txbyte = (txbyte >> 0x01) + 0x8000;                         // Двигаем txbyte вправо на 1 и пишем 1 в старший разряд (0x8000)
    if(txbitcount > 0)                                          // Если идет передача (счетик бит больше нуля),
    {
        txbitcount--;                                           // то уменьшаем его на единицу.
    }
}

/*
*   ISR(TIM0_COMPB_vect)
*   Прерывание таймера по сравнению с регистром OCR0B. Оно работает аналогично прерыванию ISR(TIM0_COMPA_vect),
*   но, в отличие от него, при выполнении этого прерывания не происходит обнуления таймера TCNT0. Это прерывание
*   разрешается только когда мы принимаем данные, в остальное время это прерывание запрещено. Когда оно случается,
*   мы проверяем логическое состояние входа UART RXD и, если оно в единице, то пишем единицу в старший разряд
*   переменной приема rxbyte, затем мы уменьшаем на единицу счетчик принятых бит и, если он стал нулем, заканчиваем
*   прием. Иначе сдвигаем вправо переменную rxbyte, чтобы подготовить ее к приему следующего бита.
*/

ISR(TIM0_COMPB_vect)
{
    if(RXPORT & (1 << RXD))         // Проверяем в каком состоянии вход RXD
        rxbyte |= 0x80;             // Если в 1, то пишем 1 в старший разряд rxbyte
    
    if(--rxbitcount == 0)           // Уменьшаем на 1 счетчик бит и проверяем не стал ли он нулем
    {
        TIMSK0 &= ~(1 << OCIE0B);   // Если да, запрещаем прерывание по сравнению OCR0B
        TIFR0 |= (1 << OCF0B);      // Очищаем флаг прерывания (важно!)
        GIFR |= (1 << INTF0);       // Очищаем флаг прерывания по INT0
        GIMSK |= (1 << INT0);       // Разрешаем прерывание INT0
    }
    else
    {
        rxbyte >>= 0x01;            // Иначе сдвигаем вправо на 1 rxbyte
    }
}

/*
*   ISR(INT0_vect)
*   Прерывание INT0. Срабатывает по заднему фронту испульса на входе INT0, используется
*   для отслеживания начала приема информации. Выставляет счетчик бит равным 9, обнуляет
*   содержимое переменной rxbyte. Задает значение для регистра OCR0B, задающего периодичность
*   срабатывания прерывания ISR(TIM0_COMPB_vect), оно должно приходиться на середину принимаемого
*   бита (по времени). После чего прерывание ISR(TIM0_COMPB_vect) разрешается, а прерывание INT0
*   запрещается.
*/

ISR(INT0_vect)
{
    rxbitcount = 0x09;                      // 8 бит данных и 1 стартовый бит
    rxbyte = 0x00;                          // Обнуляем содержимое rxbyte
    if(TCNT0 < (BAUD_DIV / 2))              // Если таймер не досчитал до середины текущего периода
    {
        OCR0B = TCNT0 + (BAUD_DIV / 2); // То прерывание произойдет в текущем периоде спустя пол периода
    }
    else
    {
        OCR0B = TCNT0 - (BAUD_DIV / 2); // Иначе прерывание произойдет в уже следующем периоде таймера
    }
    GIMSK &= ~(1 << INT0);                  // Запрещаем прерывание по INT0
    TIFR0 |= (1 << OCF0A) | (1 << OCF0B);   // Очищаем флаг прерывания INT0
    TIMSK0 |= (1 << OCIE0B);                // Разрешаем прерывание по OCR0B
}

/*
*   void uart_send(uint8_t tb)
*   Функция передачи байта по UART. Принимает в качестве аргумента байт для передачи, возвращаемого значения нет.
*   Если в данный момент идет передача байта, то она ждет пока передача закончится после чего записывает в
*   младшие 8 бит переменной txbyte байт для передачи, старшие 8 бит остаются 0xFF, затем сдвигает переменную
*   влево, создавая таким образом стартовый бит в младшем разряде. Задает счетчик бит = 10.
*/

void uart_send(uint8_t tb)
{
    while(txbitcount);              // Ждем пока закончится передача предыдущего байта
    txbyte = (tb + 0xFF00) << 0x01; // Пишем в младшие разряды txbyte данные для передачи и сдвигаем влево на 1
    txbitcount = 0x0A;              // Задаем счетчик байт равным 10
}

/*
*   int16_t uart_recieve(uint8_t* rb)
*   Функция приема байта по UART. Принимает в аргумент указатель на 8-битную переменную, где будет содержаться
*   принятый байт. Возвращает принятый байт, если байт принят, возвращает (-1), если принимать нечего.
*   Если в момент вызова функции идет прием, функция будет ждать его завершения. Если функцию вызвать дважды,
*   то первый раз она возвратит принятый байт, второй раз (-1).
*/

int16_t uart_recieve(uint8_t* rb)
{
    if(rxbitcount < 0x09)   // Если счетчик бит на прием меньше 9
    {
        while(rxbitcount);  // Ждем пока завершится текущий прием
        *rb = rxbyte;       // Пишем по адресу указателя принятый байт
        rxbitcount = 0x09;  // Восстанавливаем значение счетчика бит
        return (*rb);       // Возвращаемся
    }
    else
    {
        return (-1);        // Иначе возвращаем -1 (принимать нечего)
    }
}

/*
*   void uart_init()
*   Функция инициализации UART. Аргументов нет, возвращаемого значения нет.
*   Инициализирует глобальные переменные и регистры микроконтроллера.
*/

void uart_init()
{
    txbyte = 0xFFFF;        // Значение буфера на передачу - все единицы
    rxbyte = 0x00;          // Значение буфера на прием - все нули
    txbitcount = 0x00;      // Значение счетчика преедаваемых бит - ноль (ничего пока не передаем)
    rxbitcount = 0x09;      // Значение счетчика бит на прием - 9 (ожидаем возможного приема)
    
    TXDDR |= (1 << TXD);        // Задаем направление порта на передачу как выход
    RXDDR &= ~(1 << RXD);       // Задаем направление порта на прием как вход
    TXPORT |= (1 << TXD);       // Пишем единицу в выход TXD
    RXPORT |= (1 << RXD);       // Подтягиваем к единице вход RXD
    OCR0A = BAUD_DIV;           // Задаем значение регистра OCR0A в соответствии с бодрейтом
    TIMSK0 |= (1 << OCIE0A);    // Разрешаем прерывание TIM0_COMPA
    TCCR0A |= (1 << WGM01);     // Режим таймера CTC (очистка TCNT0 по достижению OCR0A)
    TCCR0B |= T_DIV;            // Задаем скорость счета таймера в соответствии с делителем
    MCUCR |= (1 << ISC01);      // Задаем прерывание INT0 по заднему фронту импульса
    GIMSK |= (1 << INT0);       // Разрешаем прерывание INT0
    sei();                      // Разрешаем прерывания глобально
}
