import os

translations = {
    "de": {
        "tut_title": "Interaktive Anleitung",
        "tut_start_btn": "Anleitung starten",
        "tut_btn_prev": "Zurück",
        "tut_btn_next": "Weiter",
        "tut_btn_finish": "Fertigstellen",
        "tut_step1_title": "Willkommen bei BTTB",
        "tut_step1_text": "Willkommen bei der interaktiven Anleitung für Burn to the Brim! Diese Anleitung führt Sie durch die wichtigsten Funktionen, ohne Aktionen auszuführen. Klicken Sie auf Weiter, um zu beginnen.",
        "tut_step2_title": "Quellverzeichnisse",
        "tut_step2_text": "Quellordner: Klicken Sie auf '+', um mehrere Verzeichnisse zu verwalten, oder geben Sie Ordnerpfade durch Semikolons getrennt ein. Dateien in diesen Verzeichnissen werden organisiert.",
        "tut_step3_title": "Zielverzeichnis",
        "tut_step3_text": "Zielordner: Geben Sie das Zielverzeichnis an, in dem BTTB die gepackten Datenträgerordner (z. B. Volume_1, Volume_2) erstellt.",
        "tut_step4_title": "Semantischer Prompt",
        "tut_step4_text": "Semantischer Prompt: Geben Sie Verpackungsregeln in natürlicher Sprache ein (z. B. 'Musikdateien gruppieren'), um die Organisation mithilfe von neuronalen Embeddings zu beeinflussen.",
        "tut_step5_title": "Verpackungsoptionen",
        "tut_step5_text": "Optionen: Wählen Sie zwischen Verschieben (verschiebt Dateien) oder Symlink (zerstörungsfreie virtuelle Verknüpfungen). Sie können auch Spanning und Protokolle aktivieren.",
        "tut_step6_title": "Ergebnis-Explorer",
        "tut_step6_text": "Ergebnis-Explorer: Die Baumstruktur zeigt, wie Elemente den Volumes zugewiesen sind. Klicken Sie mit der rechten Maustaste auf ein Volume oder eine Datei, um sie an ihrem ursprünglichen Ort wiederherzustellen.",
        "tut_step7_title": "Testen und Starten",
        "tut_step7_text": "Testen & Starten: Klicken Sie auf 'Testen', um eine sichere Simulation auszuführen, ohne Dateien zu ändern, oder auf 'Starten', um die tatsächliche Dateiorganisation zu beginnen."
    },
    "el": {
        "tut_title": "Διαδραστικό Σεμινάριο",
        "tut_start_btn": "Έναρξη Σεμιναρίου",
        "tut_btn_prev": "Πίσω",
        "tut_btn_next": "Επόμενο",
        "tut_btn_finish": "Τέλος",
        "tut_step1_title": "Καλώς ορίσατε στο BTTB",
        "tut_step1_text": "Καλώς ορίσατε στο διαδραστικό σεμινάριο του Burn to the Brim! Αυτός ο οδηγός θα σας καθοδηγήσει στις βασικές λειτουργίες χωρίς να εκτελέσει καμία ενέργεια. Κάντε κλικ στο Επόμενο για να ξεκινήσετε.",
        "tut_step2_title": "Κατάλογοι Πηγής",
        "tut_step2_text": "Φάκελος Πηγής: Κάντε κλικ στο '+' για να διαχειριστείτε πολλούς καταλόγους ή εισαγάγετε διαδρομές φακέλων που χωρίζονται με ερωτηματικά. Τα αρχεία μέσα σε αυτούς τους καταλόγους θα οργανωθούν.",
        "tut_step3_title": "Κατάλογος Προορισμού",
        "tut_step3_text": "Φάκελος Προορισμού: Καθορίστε τον κατάλογο προορισμού όπου το BTTB θα δημιουργήσει καταλόγους πακέτων τόμων (π.χ. Volume_1, Volume_2).",
        "tut_step4_title": "Σημασιολογική Προτροπή",
        "tut_step4_text": "Σημασιολογική Προτροπή: Εισαγάγετε κανόνες συσκευασίας σε φυσική γλώσσα (π.χ. 'ομαδοποίηση αρχείων ήχου') για να επηρεάσετε την οργάνωση χρησιμοποιώντας νευρωνικά ενσωματώματα.",
        "tut_step5_title": "Επιλογές Συσκευασίας",
        "tut_step5_text": "Επιλογές: Επιλέξτε μεταξύ Μετακίνησης (μεταφορά αρχείων) ή Symlink (μη καταστροφικοί εικονικοί σύνδεσμοι). Μπορείτε επίσης να ενεργοποιήσετε την κάλυψη τόμων και τα αρχεία καταγραφής.",
        "tut_step6_title": "Εξερεύνηση Αποτελεσμάτων",
        "tut_step6_text": "Εξερεύνηση Αποτελεσμάτων: Το δέντρο δείχνει πώς αντιστοιχίζονται τα στοιχεία στους τόμους. Κάντε δεξί κλικ σε οποιονδήποτε τόμο ή αρχείο για να ανοίξετε το μενού και να το επαναφέρετε.",
        "tut_step7_title": "Δοκιμή και Έναρξη",
        "tut_step7_text": "Δοκιμή & Έναρξη: Κάντε κλικ στο 'Δοκιμή' για να εκτελέσετε μια ασφαλή προσομοίωση χωρίς να αγγίξετε αρχεία ή κάντε κλικ στο 'Έναρξη' για να ξεκινήσει η πραγματική διαδικασία."
    },
    "elv": {
        "tut_title": "Interactive Tutorial",
        "tut_start_btn": "Start Tutorial",
        "tut_btn_prev": "Back",
        "tut_btn_next": "Next",
        "tut_btn_finish": "Alassë (Finish)",
        "tut_step1_title": "Welcome to BTTB",
        "tut_step1_text": "Welcome to the BTTB interactive tutorial! This guide will walk you through the key features without running any actions. Click Next to begin.",
        "tut_step2_title": "Source Directories",
        "tut_step2_text": "Source Folder: Specify one or more directories containing the files you wish to pack. You can click '+' to manage multiple folders.",
        "tut_step3_title": "Target Directory",
        "tut_step3_text": "Target Folder: Specify the destination directory where BTTB will create packed volume directories (e.g., Volume_1, Volume_2).",
        "tut_step4_title": "Semantic Prompt",
        "tut_step4_text": "Semantic Prompt: Enter natural language hints (e.g. 'keep family photos together') to influence the packing using text embeddings.",
        "tut_step5_title": "Packing Options",
        "tut_step5_text": "Options: Choose between Move (relocates files) or Symlink (non-destructive virtual links). You can also enable volume spanning and logs.",
        "tut_step6_title": "Results Explorer",
        "tut_step6_text": "Results Explorer: The tree shows how items are assigned to volumes. Right-click any volume or file to open the context menu and restore it.",
        "tut_step7_title": "Test and Start",
        "tut_step7_text": "Test & Start: Click 'Test' to run a safe packing simulation without touching files, or click 'Start' to perform the actual file placement."
    },
    "es": {
        "tut_title": "Tutorial Interactivo",
        "tut_start_btn": "Iniciar Tutorial",
        "tut_btn_prev": "Atrás",
        "tut_btn_next": "Siguiente",
        "tut_btn_finish": "Finalizar",
        "tut_step1_title": "Bienvenido a BTTB",
        "tut_step1_text": "¡Bienvenido al tutorial interactivo de Burn to the Brim! Esta guía le mostrará las funciones clave sin ejecutar ninguna acción. Haga clic en Siguiente para comenzar.",
        "tut_step2_title": "Directorios de Origen",
        "tut_step2_text": "Carpeta de Origen: Haga clic en '+' para administrar varios directorios, o introduzca rutas separadas por punto y coma. Se organizarán los archivos de estas carpetas.",
        "tut_step3_title": "Directorio de Destino",
        "tut_step3_text": "Carpeta de Destino: Especifique la carpeta de destino donde BTTB creará las carpetas de volúmenes (por ejemplo, Volume_1, Volume_2).",
        "tut_step4_title": "Indicación Semántica",
        "tut_step4_text": "Indicación Semántica: Introduzca reglas en lenguaje natural (por ejemplo, 'agrupar archivos de música') para guiar la organización mediante incrustaciones neuronales.",
        "tut_step5_title": "Opciones de Organización",
        "tut_step5_text": "Opciones: Elija entre Mover (mueve archivos) o Symlink (enlaces virtuales no destructivos). También puede activar la división en volúmenes y el registro de depuración.",
        "tut_step6_title": "Explorador de Resultados",
        "tut_step6_text": "Explorador: El árbol muestra cómo se asignan los elementos a los volúmenes. Haga clic con el botón derecho en un volumen o archivo para restaurarlo a su ubicación original.",
        "tut_step7_title": "Probar e Iniciar",
        "tut_step7_text": "Probar e Iniciar: Haga clic en 'Probar' para simular la organización de forma segura sin tocar archivos, o haga clic en 'Iniciar' para comenzar el proceso real."
    },
    "fr": {
        "tut_title": "Tutoriel Interactif",
        "tut_start_btn": "Démarrer le tutoriel",
        "tut_btn_prev": "Précédent",
        "tut_btn_next": "Suivant",
        "tut_btn_finish": "Terminer",
        "tut_step1_title": "Bienvenue sur BTTB",
        "tut_step1_text": "Bienvenue dans le tutoriel interactif de Burn to the Brim ! Ce guide vous présentera les fonctionnalités clés sans exécuter d'actions. Cliquez sur Suivant pour commencer.",
        "tut_step2_title": "Dossiers Sources",
        "tut_step2_text": "Dossier Source : Cliquez sur '+' pour gérer plusieurs répertoires, ou entrez des chemins séparés par des points-virgules. Les fichiers de ces répertoires seront organisés.",
        "tut_step3_title": "Dossier Cible",
        "tut_step3_text": "Dossier Cible : Spécifiez le répertoire de destination où BTTB créera les dossiers de volumes (ex. Volume_1, Volume_2).",
        "tut_step4_title": "Invite Sémantique",
        "tut_step4_text": "Invite Sémantique : Entrez des règles en langage naturel (ex. 'regrouper les fichiers audio') pour influencer l'organisation via des plongements neuronaux.",
        "tut_step5_title": "Options d'Organisation",
        "tut_step5_text": "Options : Choisissez entre Déplacer (déplace les fichiers) ou Symlink (liens virtuels non destructifs). Vous pouvez aussi activer la répartition et les journaux.",
        "tut_step6_title": "Explorateur de Résultats",
        "tut_step6_text": "Explorateur : L'arborescence montre la répartition des éléments sur les volumes. Faites un clic droit sur un volume ou fichier pour le restaurer à son emplacement d'origine.",
        "tut_step7_title": "Tester et Démarrer",
        "tut_step7_text": "Tester & Démarrer : Cliquez sur 'Tester' pour exécuter une simulation d'organisation sans modifier de fichiers, ou sur 'Démarrer' pour lancer l'organisation réelle."
    },
    "hi": {
        "tut_title": "इंटरैक्टिव ट्यूटोरियल",
        "tut_start_btn": "ट्यूटोरियल शुरू करें",
        "tut_btn_prev": "पीछे",
        "tut_btn_next": "आगे",
        "tut_btn_finish": "समाप्त",
        "tut_step1_title": "BTTB में आपका स्वागत है",
        "tut_step1_text": "Burn to the Brim इंटरैक्टिव ट्यूटोरियल में आपका स्वागत है! यह गाइड आपको बिना कोई कार्रवाई किए मुख्य विशेषताओं के बारे में बताएगी। शुरू करने के लिए आगे पर क्लिक करें।",
        "tut_step2_title": "स्रोत निर्देशिकाएँ",
        "tut_step2_text": "स्रोत फ़ोल्डर: एकाधिक निर्देशिकाओं को प्रबंधित करने के लिए '+' पर क्लिक करें, या अर्धविराम से अलग किए गए पथ दर्ज करें। इन फ़ोल्डरों की फाइलें व्यवस्थित की जाएंगी।",
        "tut_step3_title": "लक्ष्य निर्देशिका",
        "tut_step3_text": "लक्ष्य फ़ोल्डर: गंतव्य निर्देशिका निर्दिष्ट करें जहां BTTB वॉल्यूम फ़ोल्डर (जैसे Volume_1, Volume_2) बनाएगा।",
        "tut_step4_title": "सिमेंटिक प्रॉम्प्ट",
        "tut_step4_text": "सिमेंटिक प्रॉम्प्ट: तंत्रिका एम्बेडिंग का उपयोग करके संगठन को प्रभावित करने के लिए प्राकृतिक भाषा पैकिंग नियम (जैसे 'ऑडियो फ़ाइलों को समूहित करें') दर्ज करें।",
        "tut_step5_title": "पैकिंग विकल्प",
        "tut_step5_text": "विकल्प: ले जाने (फ़ाइलों को स्थानांतरित करना) या सिम्लिंक (गैर-विनाशकारी वर्चुअल लिंक) के बीच चयन करें। आप वॉल्यूम स्पैनिंग और लॉग भी सक्षम कर सकते हैं।",
        "tut_step6_title": "परिणाम एक्सप्लोरर",
        "tut_step6_text": "परिणाम एक्सप्लोरर: ट्री दिखाता है कि आइटम वॉल्यूम को कैसे सौंपे गए हैं। फ़ाइल या वॉल्यूम को उसके मूल स्थान पर पुनर्स्थापित करने के लिए राइट-क्लिक करें।",
        "tut_step7_title": "परीक्षण और प्रारंभ",
        "tut_step7_text": "परीक्षण और प्रारंभ: फ़ाइलों को प्रभावित किए बिना सुरक्षित पैकिंग सिमुलेशन चलाने के लिए 'परीक्षण' पर क्लिक करें, या वास्तविक संगठन प्रक्रिया शुरू करने के लिए 'प्रारंभ' पर क्लिक करें।"
    },
    "it": {
        "tut_title": "Tutorial Interattivo",
        "tut_start_btn": "Avvia Tutorial",
        "tut_btn_prev": "Indietro",
        "tut_btn_next": "Avanti",
        "tut_btn_finish": "Fine",
        "tut_step1_title": "Benvenuto in BTTB",
        "tut_step1_text": "Benvenuto nel tutorial interattivo di Burn to the Brim! Questa guida ti guiderà attraverso le funzioni chiave senza eseguire alcuna azione. Clicca su Avanti per iniziare.",
        "tut_step2_title": "Directory Sorgente",
        "tut_step2_text": "Cartella Sorgente: Fai clic su '+' per gestire più directory o inserisci percorsi separati da punti e virgola. I file all'interno verranno organizzati.",
        "tut_step3_title": "Directory di Destinazione",
        "tut_step3_text": "Cartella Destinazione: Specifica la directory in cui BTTB creerà le cartelle dei volumi (es. Volume_1, Volume_2).",
        "tut_step4_title": "Suggerimento Semantico",
        "tut_step4_text": "Suggerimento Semantico: Inserisci regole in linguaggio naturale (es. 'raggruppa file musicali') per influenzare l'organizzazione tramite embeddings neurali.",
        "tut_step5_title": "Opzioni di Packing",
        "tut_step5_text": "Opzioni: Scegli tra Sposta (sposta i file) o Symlink (collegamenti virtuali non distruttivi). Puoi anche abilitare la suddivisione in volumi e i log.",
        "tut_step6_title": "Esplora Risultati",
        "tut_step6_text": "Esplora Risultati: L'albero mostra l'assegnazione degli elementi ai volumi. Fai clic con il tasto destro su un volume o file per ripristinarlo nella posizione originale.",
        "tut_step7_title": "Test e Avvio",
        "tut_step7_text": "Test & Avvio: Fai clic su 'Test' per avviare una simulazione sicura senza modificare i file, o fai clic su 'Avvia' per iniziare il processo reale."
    },
    "ja": {
        "tut_title": "インタラクティブチュートリアル",
        "tut_start_btn": "チュートリアルを開始",
        "tut_btn_prev": "戻る",
        "tut_btn_next": "次へ",
        "tut_btn_finish": "完了",
        "tut_step1_title": "BTTBへようこそ",
        "tut_step1_text": "Burn to the Brim インタラクティブ チュートリアルへようこそ！このガイドでは、実際のアクションを実行することなく、主な機能について説明します。[次へ]をクリックして開始します。",
        "tut_step2_title": "元のディレクトリ",
        "tut_step2_text": "ソースフォルダー：'+' をクリックして複数のディレクトリを管理するか、セミコロンで区切られたパスを入力します。これらのフォルダ内のファイルが整理されます。",
        "tut_step3_title": "保存先ディレクトリ",
        "tut_step3_text": "ターゲットフォルダー：BTTB が分割ボリュームフォルダ (Volume_1、Volume_2 など) を作成する宛先ディレクトリを指定します。",
        "tut_step4_title": "セマンティックプロンプト",
        "tut_step4_text": "セマンティックプロンプト：自然言語のルール（例: '音楽ファイルをグループ化する'）を入力し、ニューラル埋め込みを使用してファイル配置に影響を与えます。",
        "tut_step5_title": "パッキングオプション",
        "tut_step5_text": "オプション：ファイル移動（ファイルを移動）またはシンボリックリンク（非破壊的な仮想リンク）のいずれかを選択します。ボリュームスパンと診断ログを有効にすることもできます。",
        "tut_step6_title": "結果エクスプローラー",
        "tut_step6_text": "結果エクスプローラー：ツリーは、各項目がボリュームにどのように割り当てられているかを示します。ボリュームまたはファイルを右クリックして、元の場所に復元できます。",
        "tut_step7_title": "テストと開始",
        "tut_step7_text": "[テスト]をクリックして、ファイルを変更せずに安全なパッキングシミュレーションを実行するか、[開始]をクリックして実際の配置プロセスを開始します。"
    },
    "la": {
        "tut_title": "Tutela Interactiva",
        "tut_start_btn": "Incipere Tutelam",
        "tut_btn_prev": "Retro",
        "tut_btn_next": "Deinde",
        "tut_btn_finish": "Finis",
        "tut_step1_title": "Salvetis ad BTTB",
        "tut_step1_text": "Salvetis ad Burn to the Brim interactive tutela! Hic dux te per praecipuas functiones sine ulla actione perficienda ducet. Clicca super Deinde ad incipiendum.",
        "tut_step2_title": "Directoria Fontium",
        "tut_step2_text": "Capsa Fontis: Clicca '+' ad plura directoria regenda, vel scribe tramites punctis ac virgulis divisos. Fasciculi ordinabuntur.",
        "tut_step3_title": "Directorium Scopi",
        "tut_step3_text": "Capsa Scopi: Scribe capsam destinationis ubi BTTB directoria voluminum (ex. Volume_1, Volume_2) creabit.",
        "tut_step4_title": "Promptum Semanticum",
        "tut_step4_text": "Promptum Semanticum: Scribe regulas sermone nativo (ex. 'group musicam') ut ordinationem per insertiones neurales dirigas.",
        "tut_step5_title": "Optiones Inpactionis",
        "tut_step5_text": "Optiones: Elige inter Movere (fasciculos transfert) vel Symlink (nexus virtuales non destructivi). Spanning et log-codices quoque excitare potes.",
        "tut_step6_title": "Explorator Eventuum",
        "tut_step6_text": "Explorator: Arbor ostendit quomodo fasciculi voluminibus attribuantur. Dextra-clicca super volumen vel fasciculum ad locum originalem restituendum.",
        "tut_step7_title": "Temptare et Incipere",
        "tut_step7_text": "Temptare & Incipere: Clicca 'Temptare' ut simulationem facias sine fasciculis mutandis, vel 'Incipere' ut rem ipsam incipias."
    },
    "nl": {
        "tut_title": "Interactieve Handleiding",
        "tut_start_btn": "Handleiding Starten",
        "tut_btn_prev": "Vorige",
        "tut_btn_next": "Volgende",
        "tut_btn_finish": "Voltooien",
        "tut_step1_title": "Welkom bij BTTB",
        "tut_step1_text": "Welkom bij de interactieve handleiding van Burn to the Brim! Deze gids loodst u door de belangrijkste functies zonder acties uit te voeren. Klik op Volgende om te beginnen.",
        "tut_step2_title": "Bronmappen",
        "tut_step2_text": "Bronmap: Klik op '+' om meerdere mappen te beheren, of voer paden in gescheiden door puntkomma's. Bestanden in deze mappen worden georganiseerd.",
        "tut_step3_title": "Doelmap",
        "tut_step3_text": "Doelmap: Geef de doelmap op waarin BTTB de volume-mappen (bijv. Volume_1, Volume_2) zal aanmaken.",
        "tut_step4_title": "Semantische Prompt",
        "tut_step4_text": "Semantische Prompt: Voer regels in in natuurlijke taal (bijv. 'groepeer audiobestanden') om de verdeling te sturen met behulp van neurale netwerken.",
        "tut_step5_title": "Inpakopties",
        "tut_step5_text": "Inpakopties: Kies tussen Verplaatsen (verplaatst bestanden) of Symlink (virtuele snelkoppelingen). U kunt ook spanning over meerdere volumes en logboeken inschakelen.",
        "tut_step6_title": "Resultatenverkenner",
        "tut_step6_text": "Resultatenverkenner: De boomstructuur toont hoe bestanden zijn verdeeld over de volumes. Klik met de rechtermuisknop op een volume of bestand om het te herstellen naar de originele locatie.",
        "tut_step7_title": "Testen en Starten",
        "tut_step7_text": "Testen & Starten: Klik op 'Testen' om een veilige simulatie uit te voeren zonder bestanden te wijzigen, of klik op 'Starten' om de daadwerkelijke bestandsverplaatsing te starten."
    },
    "pt": {
        "tut_title": "Tutorial Interativo",
        "tut_start_btn": "Iniciar Tutorial",
        "tut_btn_prev": "Voltar",
        "tut_btn_next": "Avançar",
        "tut_btn_finish": "Concluir",
        "tut_step1_title": "Bem-vindo ao BTTB",
        "tut_step1_text": "Bem-vindo ao tutorial interativo do Burn to the Brim! Este guia irá guiá-lo pelas principais funcionalidades sem executar nenhuma ação. Clique em Avançar para começar.",
        "tut_step2_title": "Diretórios de Origem",
        "tut_step2_text": "Pasta de Origem: Clique em '+' para gerenciar múltiplos diretórios, ou insira caminhos separados por ponto e vírgula. Arquivos nestas pastas serão organizados.",
        "tut_step3_title": "Diretório de Destino",
        "tut_step3_text": "Pasta de Destino: Especifique a pasta de destino onde o BTTB criará as pastas de volumes (ex: Volume_1, Volume_2).",
        "tut_step4_title": "Prompt Semântico",
        "tut_step4_text": "Prompt Semântico: Digite regras em linguagem natural (ex: 'agrupar arquivos de som') para influenciar a organização usando embeddings neurais.",
        "tut_step5_title": "Opções de Organização",
        "tut_step5_text": "Opções: Escolha entre Mover (move os arquivos) ou Symlink (links virtuais não destrutivos). Você também pode habilitar a divisão em volumes e logs.",
        "tut_step6_title": "Explorador de Resultados",
        "tut_step6_text": "Explorador: A árvore mostra a atribuição dos itens aos volumes. Clique com o botão direito em um volume ou arquivo para restaurá-lo ao local original.",
        "tut_step7_title": "Testar e Iniciar",
        "tut_step7_text": "Testar e Iniciar: Clique em 'Testar' para rodar uma simulação de organização sem tocar nos arquivos, ou 'Iniciar' para começar o processo real."
    },
    "vul": {
        "tut_title": "Logic Tutorial",
        "tut_start_btn": "Start Tutorial",
        "tut_btn_prev": "Back",
        "tut_btn_next": "Next",
        "tut_btn_finish": "Spokh (Finish)",
        "tut_step1_title": "Welcome to BTTB",
        "tut_step1_text": "Welcome to the logical interactive tutorial! This guide will walk you through all key structures without executing any action. Click Next to begin.",
        "tut_step2_title": "Source Directories",
        "tut_step2_text": "Source Folder: Select directories containing logical files t'pack. Multiple sources can be managed via the '+' control.",
        "tut_step3_title": "Target Directory",
        "tut_step3_text": "Target Folder: Specify the target destination where solved volumes (voh-lar) will be written.",
        "tut_step4_title": "Semantic Prompt",
        "tut_step4_text": "Semantic Prompt: Guide the logic solver via natural language prompt parameters using MiniLM embeddings.",
        "tut_step5_title": "Packing Options",
        "tut_step5_text": "Options: Choose between Move (reposition files) or Symlink (non-destructive logical links). Spanning and Tracing can also be configured.",
        "tut_step6_title": "Results Explorer",
        "tut_step6_text": "Explorer: Renders the computed voh-lar structures. Right-click any node t'reveal the restore-action context menu.",
        "tut_step7_title": "Test and Start",
        "tut_step7_text": "Execution: Zvhal (Test) simulates packing without disk operations. Fa-lok (Start) executes the actual organization."
    }
}

lang_dir = "/home/sander/src/antigravity/project1/bttb_cpp/lang"

for lang, data in translations.items():
    file_path = os.path.join(lang_dir, f"{lang}.po")
    if not os.path.exists(file_path):
        print(f"File {file_path} not found.")
        continue
        
    print(f"Processing {lang}.po...")
    with open(file_path, "r", encoding="utf-8") as f:
        content = f.read()
        
    updated = False
    for key, val in data.items():
        # Check if the key is already in the file
        if f'msgid "{key}"' in content:
            continue
            
        # Append msgid and msgstr
        # Double escape newlines
        val_escaped = val.replace("\n", "\\n").replace('"', '\\"')
        entry = f'\nmsgid "{key}"\nmsgstr "{val_escaped}"\n'
        content += entry
        updated = True
        
    if updated:
        with open(file_path, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"Updated {lang}.po.")
    else:
        print(f"No updates needed for {lang}.po.")
