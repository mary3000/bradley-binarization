## Бинаризация изображений. Алгоритм Бредли.

### Инструкция

Одно из двух:
- Запустить в CLion
- `./BradleyAlgo $in_dir $out_dir`

Бинарник уже есть, но на всякий случай вот команды:
```
cmake . 
make
```

### Результат
Вывод программы:
```
Processed	08.JPG, elapsed	529936 μs, performance	0.066323 s/megapixel
Processed	14.JPG, elapsed	515645 μs, performance	0.064534 s/megapixel
Processed	04.jpg, elapsed	521140 μs, performance	0.065222 s/megapixel
Processed	07.JPG, elapsed	586352 μs, performance	0.073383 s/megapixel
Processed	10.JPG, elapsed	575421 μs, performance	0.072015 s/megapixel
Processed	11.JPG, elapsed	522305 μs, performance	0.065368 s/megapixel
Processed	05.JPG, elapsed	320832 μs, performance	0.063935 s/megapixel
Processed	13.JPG, elapsed	514086 μs, performance	0.064339 s/megapixel
Processed	12.JPG, elapsed	525706 μs, performance	0.065793 s/megapixel
Processed	06.JPG, elapsed	520932 μs, performance	0.065196 s/megapixel
Processed	09.JPG, elapsed	530016 μs, performance	0.066333 s/megapixel
Processed	03.jpg, elapsed	580607 μs, performance	0.072664 s/megapixel
Processed	01.jpg, elapsed	239817 μs, performance	0.072507 s/megapixel
Processed	15.JPG, elapsed	507160 μs, performance	0.066686 s/megapixel
Processed	02.jpg, elapsed	521755 μs, performance	0.065299 s/megapixel

```
### Анализ

Алгоритм достаточно простой: локально, по небольшим блокам, считает среднее значение в уже бесцветном изображении. 
Затем каждый пиксель с коэффициентом сравнивает со средним в блоке и по порогу решает, черное или белое.

##### Преимущества:
- Очевидно, засчет локальности хорошо справляется с изображениями с неоднородным освещением (например, на 2 изображении неплохо справился с тенью);
- Судя по выводу программы, алгоритм работает достаточно быстро;
- Простой в понимании и реализации;
##### Недостатки:
- Если изображение низко-контрастное, то алгоритму может быть сложно правильно выбрать порог;
