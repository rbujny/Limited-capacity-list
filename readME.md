# Lista o ograniczonej pojemności

## Założenia

Zaimplementuj strukturę danych typu lista, przechowującą wskaźniki na dostarczone
przez użytkownika obszary pamięci (np. łańcuchy tekstowe). Lista może się dynamicznie rozrastać i kurczyć w zależności od sekwencji wywołań wewnętrznych operacji,
ale jej całkowita pojemność nie może przekroczyć zadanego N. Lista powinna umożliwiać bezpieczne, współbieżne przetwarzanie wielowątkowe: dowolna liczba wątków
może w dowolnym momencie wywoływać dowolne funkcje listy.
---

## Funkcje

`void put(void *el)`

`void* get()`

`int removeElement(void *el)`

`int getCount()`

`void setMaxSize(int n)`

---
## Funkcje pomocnicze

`void *testFunction(void *arg)` - funkcja testująca wcześniej wymienione funkcje. Używa wątki do symulacji wielowątkowości,które wraz z zamkiem (mutexem) zostały utworzone w funkcji `int main()`.


### ✍️ Autor

*Radosław Bujny 155950*
