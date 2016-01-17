Wizualizacja ruchow chromatyny w jadrze komorkowym.

Instalacja:
    1. Sciagamy Qt open source, np. stad: http://www.qt.io/download-open-source/
    2. Instalujac Qt musimy wybrac folder instalacji - nazwijmy te sciezke qt-path
    3. Pobieramy paczke z projektem
    4. Edytujemy install.sh: QT_PATH=qt-path (sciezka do naszej instalacji qt), COMPILER=[nasz kompilator]
       gdzie [nasz kompilator] to nazwa folderu w qt-path/5.5/ ktory najlepiej nam odpowiada (w przypadku linuxa powinien to byc gcc_64)
    5. Uruchamiamy ./install.sh
    6. W folderze build znajduje sie program wykonywalny QChromosome
