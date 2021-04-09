listSize = csvread ("CRC2Hash.csv");
hash     = 0:(0xFFFF - 1);

graphic  = figure ()
bar (hash(1:(65000)), listSize(1:(65000)), 1)
%hold on
%for listId = 1:50
%    bar (hash (listId), listSize (listId), 10)
%end    
%hold off
grid on

ylabel ('Size of list')
xlabel ('Hash')

saveas (graphic, '~/Desktop/hashtable/csv/CRC.png');