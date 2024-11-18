set -e #-e exit immediately

# 1.  Создать каталог  out, если его нет, иначе удалить содержимое и сам каталог
test -d out && rm -r out
mkdir out

# 2. Перейти внутрь каталога
cd out

# 3. Создать файл me.txt с именем пользователя
whoami > me.txt

# 4. Скопить me.txt в metoo.txt
cp me.txt metoo.txt

# 5. Создать файл wchelp.txt с документацией по команде wc
man wc > wchelp.txt

# 6. Вывести содержимое файла wchelp.txt в консоль
cat wchelp.txt

# 7. Создать файл wchelp-lines.txt с количеством строк wchelp.txt
# выяснить как работает pipe, wc что выводит и почему cut -f1: count + filename
wc -l wchelp.txt | cut -d ' ' -f1 > wchelp-lines.txt

# 8. Создать файл wchelp-reversed.txt с содержимым wchelp.txt в обратном порядке
tac wchelp.txt > wchelp-reversed.txt

#9. Создать файл all.txt со содержимым всех файлов
# cat почему cat. wc
cat wchelp.txt wchelp-reversed.txt me.txt metoo.txt wchelp-lines.txt > all.txt

# 10. Создать архив result.tar со всему *.txt файлами
# -cf? -c create -f filename tar tape archive? gz -gnu zipped? cd -change directory?
tar -cf result.tar *.txt

# 11. Скомпрессовать архив
gzip result.tar

# 12. Выйти в родительский каталог
cd ../

# 13. Переместить архив в текущий каталог
test -f result.tar.gz && rm result.tar.gz
mv ./out/result.tar.gz ./

# 14. Удалить каталог out
rm -r ./out/*
rmdir out

# Доп: вывод текущей директории (print current directory)
pwd