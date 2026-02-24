# TLB2Z
Low cost BLE Advertisements Repeater into Zigbee 3.0 on TLRS825x

# Ретранслятор до 3-х BLE термометров в Zigbee 3.0 на модуле с чипом TLSR825x.

![b2z.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z.gif)

![b2z1](https://github.com/pvvx/TLB2Z/blob/master/web/b2z1.jpg)

Используется модуль **[TB-03F-KIT](https://docs.ai-thinker.com/en/blue_tooth)** или **[TB-04-KIT](https://docs.ai-thinker.com/en/blue_tooth)** ($3)

![tb-03f-kit.gif](https://github.com/pvvx/TLB2Z/blob/master/web/tb-03f-kit.gif)

Альтернативное исполнение на модуле **[TB-03F](https://docs.ai-thinker.com/en/blue_tooth)** (~$1.5):

![tb-03f.gif](https://github.com/pvvx/TLB2Z/blob/master/web/tb-03f.gif)

* Программа для настройки BLE [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee.html)
* [Начальное программирование модуля](https://github.com/pvvx/TLB2Z/tree/master/pgm)
* [Файлы прошивки и Zigbee OTA](https://github.com/pvvx/TLB2Z/tree/master/fw)

Вариант на [TS0001_TZ3000_gjrubzje](https://pvvx.github.io/TS0001_TZ3000/)

![ZigbeeSwitch](https://raw.githubusercontent.com/pvvx/pvvx.github.io/refs/heads/master/TS0001_TZ3000_fdxihpp7/img/ZigbeeSwitch.jpg)

Вариант на **[Zigbee Repeater HOBEIAN](https://pvvx.github.io/ZG-807Z/)**

![ZG-807Z](https://pvvx.github.io/ZG-807Z/img/ZG-807Z.jpg)


## Основные технические характеристики

* Прием BLE рекламы до 3-х термометров-гигрометров или детекторов движения и освещенности с переводом показаний в Zigbee.
* Рекламные объявления BLE принимаются 99.5% времени: опрос Zigbee происходит 16 мс каждые 4 секунды. Это обеспечивает минимальные потери приема от BLE устройств.
* Zigbee устройство использует 3 endpoint. По одному для каждого BLE устройства.
* Поддержка Zigbee OTA.
* Поддержка рекламных BLE форматов Xiaomi(Mi-Home), Qingping, BTHome v2, Custom.
* Поддержка вариантов зашифрованной рекламы BLE с помощью bindkey.
* Поддержка триггера BLE вкл/выкл (настроенного в BThome в прошивках [ATC_MiThermometer](https://github.com/pvvx/ATC_MiThermometer?tab=readme-ov-file#temperature-or-humidity-trigger-gpio-pc4-lywsd03mmc-label-on-the-p9-pin), и датчиков движения Xiaomi и Qingping.
* Поддержка прямого binding для On/Off (передача события On/Off дублируется).
* Zigbee TX +10 дБм, BLE TX +10 дБм (BLE используется только для настройки).
* Среднее потребление при работе модуля TB-03F-KIT от USB (5V): 10 мА

## Настройка модуля

1.	Подключите запрограммированный модуль к источнику питания.
2.	Загрузите [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee.html)
3.	Коротко нажмите кнопку с надписью “PROG”. Это действие переключит модуль на работу в режим ожидания подключения по BLE. Время ожидания BLE подключения до 80 секунд.
4.	В [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee.html) произведите подключение к BLE устройству “B2Z-xxxxxx”.
5.	Введите MAC термометров и по необходимости используемые BindKey.
6.	Проследите, что данные принимаются:

![b2z_html.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z_html.gif)

7.	Закройте [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee.html).

## Сопряжение модуля с Zigbee

1.	Включите режим сопряжения на Zigbee координаторе или роутере.
2.	Нажмите и удерживайте кнопку на модуле с надписью “PROG” в течении 8 секунд. Это действие сбросит привязки и настройки Zigbee.
3.	Проследите сопряжение необходимых кластеров. Установите желаемые значения для “Report”.

![b2z_zha.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z_zha.gif)

![grf.gif](https://github.com/pvvx/TLB2Z/blob/master/web/grf.gif)


## LED индикация

### Светодиод RGB коротко мигает при приеме данных от BLE устройств.

* Красный цвет светодиода – прием от 1-го BLE устройства
* Зеленый цвет светодиода – прием от 2-го BLE устройства
* Синий цвет светодиода – прием от 3-го BLE устройства

### Белый светодиод отображает состояние Zigbee. 

* Медленно мигает, если сопряжения нет
* Быстро мигает при сопряжении
* Мигает по команде “Identify”

### Желтый светодиод отображает состояние BLE. 

* Коротко мигает при ожидании BLE подключения

## TLB2Z Reportable:

| Endpoint | Cluster | Attribute | NN BLE device |
| --- | --- | --- | --- |
| 1 | 0x0001 Power Configuration | 0x0020 BatteryVoltage | 1 |
| 1 | 0x0001 Power Configuration | 0x0021 BatteryPercentageRemaining | 1 |
| 1 | 0x0001 Power Configuration | 0x0040 BatteryVoltage | 2 |
| 1 | 0x0001 Power Configuration | 0x0041 BatteryPercentageRemaining | 2 |
| 1 | 0x0001 Power Configuration | 0x0060 BatteryVoltage | 3 |
| 1 | 0x0001 Power Configuration | 0x0061 BatteryPercentageRemaining | 3 |
| 1 | 0x0006 On/Off | 0x0000 OnOff | 1 |
| 1 | 0x0400 Illuminance Measurement | 0x0000 MeasuredValue | 1 |
| 1 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 1 |
| 1 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 1 |
| 2 | 0x0006 On/Off | 0x0000 OnOff | 2 |
| 2 | 0x0400 Illuminance Measurement | 0x0000 MeasuredValue | 2 |
| 2 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 2 |
| 2 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 2 |
| 3 | 0x0006 On/Off | 0x0000 OnOff | 3 |
| 3 | 0x0400 Illuminance Measurement | 0x0000 MeasuredValue | 3 |
| 3 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 3 |
| 3 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 3 |

* Если используется менее 3-х BLE термометров-гигрометров или термометр-гигрометр не передает какой параметр, тогда нет необходимости связывать данные кластеры в Zigbee координаторе.

### Default Report Setting:

| Name | Tmin | Tmax | Change |
| --- | --- | --- | --- |
| OnOff | 0 sec | 3600 sec | 0 |
| BatteryVoltage | 360 sec | 3600 sec | 0 |
| BatteryPercentageRemaining | 360 sec | 3600 sec | 0 |
| Illuminance Measurement | 30 sec | 180 sec | 0 |
| Temperature Measurement | 30 sec | 180 sec | 10 (0.1 C) |
| Relative Humidity Measurement | 30 sec | 180 sec | 50 (0.5 %) |

* Xiaomi термометры-гигрометры используют большой интервал передачи данных, длительностями в десятки минут. Для исключения лишних дублирующих точек на графиках желательно устанавливать соответствующие (кратные) интервалы в Zigbee репорт.
