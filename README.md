# Transport Network
*Used programming language : C++*\
*Created by Kristina Dimitrova, 2group, 2 year, Computer Science*

## BasicFunctions module
В този модул са включени само общи функции, които не са свързани със самата функционалност на транспортната мрежа, а са повече за изграждане на самата транспортна мрежа и за удобство на потребителя.

### void displayStopsWithIds()
Тази функция принтира имената на всички спирки в конкретната транспортна мрежа и техните id-та.

### void displayStopSchedule(const int &stopId)
Тази функция приема като аргумент id на спирка (което може да се разбере от displayStopsWithIds()) и извежда всички автобуси, които минават през тази спирка и времената, в които се случва това.

### void displayBusSchedule(const int &busNumber)
Тази функция по подаден номер на автобус принтира спирките от маршрута му в последователността, в която минава през тях. Освен това за всяка от спирките принтира времената, в които пристига/заминава на тях.

### void displayStopBusSchedule(const int &stopId, const int &busNumber)
Тази функция приема като аргументи id на спирка и номер на автобус. Тя ни извежда времената, в които дадения автобус минава през дадената спирка.

### void serialize(const std::string &fileName)
Тази функция приема като аргумент име на файл. Тя преминава през транспортната мрежа и я записва в подходящ формат в подадения файл. Ако не съществува файл с име подаденото име, функцията създава такъв и записва данните там.

### void deserialize(const std ::string &fileName)
Тази функция приема като аргумент име на файл (в подходящ формат, който е записан по-надолу). Тя чете от него и вкарва данните в транспортната мрежа.

### void addStop(const std::string &name)
Тази функция получава име на спирка и създава спирка с такова име в транспортната мрежа. Самата програма се грижи за id-то и (чрез static int) и няма нужда потребителят да се грижи за това. Също така при създаването си през спирката не минават автобуси и следователно нямаме разписание за нея.

### void addBus(int number, const std::vector<int> &route, const std::vector<std::vector<int>> &timetable)
Тази функция приема номер на автобус, вектор от спирки представляващи маршрута на автобуса в последоваттелността в която минава през спирките и вектор от разписания. Няма проблем първата и последната спирка на автобуса да са еднакви, но единствено те могат да съвпадат. Разписанието е от тип std::vector<std::vector<int>> защото i-тия елемент е времената на пристигане/заминаване на този автобус от спирка route[i] и изискването е за всяко i, j<route.size() timetable[i].size()==timetable[j].size().

### void removeStop(const std::string &name);
Тази функция приема име на спирка, намира спирката в транспортната мрежа и я премахва. Ако има автобуси, които минават през нея, то тази спирка се изтрива от маршрутите и разписанията им. Ако тази спирка е била междинна за две други 1->х->3 (х е тази която ще изтрием) маршрутът става 1->3. Всичко друго си остава същото.

### void removeBus(int number);
Тази фунцкция има аргумент номера на автобуса, намира го в транспортната мрежа и го премахва. Премахва този автобус и времената му и от разписанията на спирките през които минава

## SpecialFunctions Module
В този модул са всички функции свързани с функционалността на транспортната мрежа, а именно извеждане на най-добрите пътища по 3-те критерия:
1. Най-бърз път от начална точка до крайна точка и тръгване в даден час
2. Път с най-малко прекачвания от начална до крайна точка и начален час
3. Път с най-малко чакане на спирка, т.е с най-кратък престой извън автобус
Всички функции в този модул приемат като аргументи начална и крайна спирка (id-тата им) и час на тръгване. Всички от тези функции връщат вектор от тип Triple, който запазва еднозначно най-добрия път по дадените критерии.

### std::vector<Triple> getMinTimeRoute(const int &startStop, const int &endStop, const int &startTime);
Тази функция използва алгоритъмът Дийкстра за изчисляване на най-къс път от стартовата спирка до всички останали. След приключване на алгоритъма възстановява най-бързия път до крайната спирка и го връща като резултат. Върховете в алгоритъма са спирките. Алгоритъмът не е използван по основния му начин с върхове и тегла на ребрата между върховете(т.е пътуване с автобус от една спирка до друга), а вместо това разглежда като тегла часовете, в които пристигаме на спирката.

### std::vector<Triple> getMinChangesRoute(const int &startStop, const int &endStop, const int &startTime);
Тази функция обхожда графът чрез BFS и конкретно чрез автобусите, които минават през спирката. Това означава, че 0-вото ниво е началната спирка, 1-вото са спирките без началната които могат да се достигнат с най-малко 1 автобус, 2-то са спирките, които могат да се достигнат с най-малко 2 автобуса и т.н. Когато срещнем крайната спирка обхождането приключва (за оптимизация на времето), възстановява се пътят изпълняващ критерий 2) и се връща като резултат.

### std::vector<Triple> getMinWaitRoute(const int &startStop, const int &endStop, const int &startTime);
Тази функция използва алгоритъмът Дийкстра за изчисляване на най-къс път по време за чакане навън от стартовата спирка до всички останали. След приключване на алгоритъма възстановява най-бързия път до крайната спирка и го връща като резултат. Тук идеята е, че можем да имаме циклични пътища като резултат, понеже можем да се возим на автобус с цикличен маршрут само за да не чакаме навън следващия автобус. Тази цикличност налага върховете в алгоритъма да са двойки от вида спирка и час. По този начин отново нямаме отрицателни ребра и цикли, но си оставяме възможността да спрем на една и съща спирка няколко пъти(но в различни часове). Алгоритъмът не е използван по основния му начин с върхове и тегла на ребрата между върховете(т.е пътуване с автобус от една спирка до друга), а вместо това теглата са времената за чакане навън.

### void minTimeRoute(const int &startStop, const int &endStop, const int &startTime);
Тази функция извиква функцията getMinTimeRoute и принтира на потребителя резултата й, но в по-четим вид и не позволява на потребителя да прави нещо със суровия вариант на резултата. По този начин може удобно да се модифицира представянето на пътя или да се получат допълнителни неща за него.

### void minChangesRoute(const int &startStop, const int &endStop, const int &startTime);
Тази функция извиква функцията getMinChangesRoute и принтира на потребителя резултата й, но в по-четим вид и не позволява на потребителя да прави нещо със суровия вариант на резултата. По този начин може удобно да се модифицира представянето на пътя или да се получат допълнителни неща за него.

### void minWaitRoute(const int &startStop, const int &endStop, const int &startTime);
Тази функция извиква функцията getMinWaitRoute и принтира на потребителя резултата й, но в по-четим вид и не позволява на потребителя да прави нещо със суровия вариант на резултата. По този начин може удобно да се модифицира представянето на пътя или да се получат допълнителни неща за него.

## Stuctures and used libraries
## stuct Stop
Тя съдържа:
 - static int id - чрез него се поддържат id-тата на автобусните спирки
 - std::string name - пази името на спирката
 - std::unordered_map<int, std::vector<int>> buses - пази номер на автобус(ключ) - вектор от часовете, в които минаа този автобус през тази спирка (стойност). Тук се пазят само автобусите, които минават през тази спирка. Това ни помага за фунцкията getMinChangesRoute и цялостно улеснява поддръжката на транспортната мрежа.

## struct Triple
Тя съдържа:
 -int stop - съхранява id-то на спирката
 -int departureBus - съхранява номера на автобуса, с който тръгваме от спирката
 -int departureTime - съхранява часа, в който тръгваме от спирката с автобуса
 -bool operator==(const Triple &other) - трябва ни за проверка за липса на маршрут (означен е с {-1,-1,-1})

## std::unordered_map<int, Stop> graph
Графа е съхранен под формата на adjacency list чрез unordered_map, защото може да сме изтриваме спирки и вече индексите им да не са последователни, а може и просто да не започват от 0, ако имаме няколко транспортни мрежи. Освен това тази структура ни връща стойност по ключ за константно време, което е една от най-използваните операции.

## std::unordered_map<int, std::vector<int>> busRoutes; 
Маршрутите на автобусите са съхранени в unordered_map, защото номерата на автобусите могат да са доста различни, а ни трябва бърз достъп до тях и маршрутите. Освен това тази структура ни връща стойност по ключ за константно време, което е една от най-използваните операции.
