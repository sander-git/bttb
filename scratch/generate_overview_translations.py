# -*- coding: utf-8 -*-
import os
import sys

# Define target directory
HTDOCS_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "../web/htdocs"))

# Define languages and their metadata
LANGUAGES = {
    "en": {"name": "English", "filename": "overview.html"},
    "es": {"name": "Español", "filename": "overview_es.html"},
    "fr": {"name": "Français", "filename": "overview_fr.html"},
    "nl": {"name": "Nederlands", "filename": "overview_nl.html"},
    "de": {"name": "Deutsch", "filename": "overview_de.html"},
    "zh": {"name": "简体中文", "filename": "overview_zh.html"},
    "ja": {"name": "日本語", "filename": "overview_ja.html"},
    "it": {"name": "Italiano", "filename": "overview_it.html"},
    "el": {"name": "Ελληνικά", "filename": "overview_el.html"},
    "la": {"name": "Latina", "filename": "overview_la.html"},
    "pt": {"name": "Português", "filename": "overview_pt.html"},
    "hi": {"name": "हिन्दी", "filename": "overview_hi.html"},
    "vul": {"name": "Vuhlkansu", "filename": "overview_vul.html"},
    "elv": {"name": "Eldarin", "filename": "overview_elv.html"}
}

# Common navbar translations
NAVBAR = {
    "en": {"home": "Home", "overview": "Overview", "news": "News", "history": "History", "download": "Download", "forum": "Forum", "contact": "Contact", "about": "About us", "help": "Help/FAQ"},
    "es": {"home": "Inicio", "overview": "Resumen", "news": "Noticias", "history": "Historial", "download": "Descargar", "forum": "Foro", "contact": "Contacto", "about": "Sobre nosotros", "help": "Ayuda/FAQ"},
    "fr": {"home": "Accueil", "overview": "Aperçu", "news": "Actualités", "history": "Historique", "download": "Télécharger", "forum": "Forum", "contact": "Contact", "about": "À propos", "help": "Aide/FAQ"},
    "nl": {"home": "Home", "overview": "Overzicht", "news": "Nieuws", "history": "Geschiedenis", "download": "Downloaden", "forum": "Forum", "contact": "Contact", "about": "Over ons", "help": "Help/FAQ"},
    "de": {"home": "Startseite", "overview": "Übersicht", "news": "Neuigkeiten", "history": "Verlauf", "download": "Herunterladen", "forum": "Forum", "contact": "Kontakt", "about": "Über uns", "help": "Hilfe/FAQ"},
    "zh": {"home": "主页", "overview": "概述", "news": "新闻", "history": "历史", "download": "下载", "forum": "论坛", "contact": "联系", "about": "关于我们", "help": "帮助/常见问题"},
    "ja": {"home": "ホーム", "overview": "概要", "news": "ニュース", "history": "履歴", "download": "ダウンロード", "forum": "フォーラム", "contact": "お問い合わせ", "about": "私たちについて", "help": "ヘルプ/FAQ"},
    "it": {"home": "Home", "overview": "Panoramica", "news": "Notizie", "history": "Cronologia", "download": "Download", "forum": "Forum", "contact": "Contatti", "about": "Chi siamo", "help": "Aiuto/FAQ"},
    "el": {"home": "Αρχική", "overview": "Επισκόπηση", "news": "Νέα", "history": "Ιστορικό", "download": "Λήψη", "forum": "Φόρουμ", "contact": "Επικοινωνία", "about": "Σχετικά με εμάς", "help": "Βοήθεια/FAQ"},
    "la": {"home": "Domus", "overview": "Conspectus", "news": "Nuntii", "history": "Historia", "download": "Demittere", "forum": "Forum", "contact": "Contactus", "about": "De nobis", "help": "Auxilium/FAQ"},
    "pt": {"home": "Início", "overview": "Visão Geral", "news": "Notícias", "history": "Histórico", "download": "Baixar", "forum": "Fórum", "contact": "Contato", "about": "Sobre nós", "help": "Ajuda/FAQ"},
    "hi": {"home": "मुख्य पृष्ठ", "overview": "अवलोकन", "news": "समाचार", "history": "इतिहास", "download": "डाउनलोड", "forum": "फोरम", "contact": "संपर्क", "about": "हमारे बारे में", "help": "सहायता/FAQ"},
    "vul": {"home": "El'ru", "overview": "Spohk", "news": "T'Zan", "history": "Spahr", "download": "Kuv-tor", "forum": "T'Kahr", "contact": "Ter-tor", "about": "Su-us", "help": "Gla-tor/FAQ"},
    "elv": {"home": "Már", "overview": "Cennas", "news": "Sinya", "history": "Gwaith", "download": "Lelta", "forum": "Oromë", "contact": "Nossa", "about": "Omentielva", "help": "Rest/FAQ"}
}

# Page content translations
CONTENT = {
    "en": {
        "title": "Burn to the Brim: Overview",
        "h1": "Application Overview",
        "subtitle": "A high-performance storage space optimizer and directory packing utility.",
        "desc_title": "Product Description",
        "desc_p1": "<strong>Burn to the Brim</strong> is a professional file packaging and storage optimization utility. It solves the classic bin-packing and subset-sum optimization challenge, allowing you to fit files and folders perfectly onto target storage mediums\u2014including recordable CDs, DVDs, Blu-rays, USB flash drives, or custom-capacity virtual drives\u2014with virtually zero wasted space.",
        "desc_p2": "Employing a highly optimized recursive backtracking solver, the application analyzes directory structures and file sizes to calculate near-perfect layout configurations in milliseconds. With a non-blocking multithreaded engine, the user interface remains completely fluid and interactive even when calculating solutions for very large datasets.",
        "features_title": "Key Features",
        "feat_span": "<strong>Multi-Volume Spanning:</strong> Automatically splits massive file hierarchies that exceed single-drive capacities into optimally sized volume folders.",
        "feat_par3": "<strong>Cauchy Reed-Solomon PAR3 Parity:</strong> Automatically creates PAR3 parity archives for error correction, factoring overhead directly into layout checks.",
        "feat_verify": "<strong>Integrity Verification &amp; Recovery:</strong> Scan and verify volumes against parity files to identify corrupted items and perform non-destructive restoration.",
        "feat_menu": "<strong>Explorer Context Menu Integration:</strong> Instantly append files and folders directly to active sessions via the Windows Explorer right-click menu.",
        "feat_cli": "<strong>Hybrid GUI/CLI Engine:</strong> Seamlessly switch between a lightweight native user interface (Win32 / GTK 4) and a full command-line mode for scripting.",
        "feat_unicode": "<strong>Unicode &amp; Long Path Support:</strong> Fully compatible with international character encodings and Windows UNC long paths up to 32,767 characters.",
        "feat_render": "<strong>Non-Blocking Tree Rendering:</strong> Displays large solution datasets incrementally using main-loop idle cycles to prevent GUI freezing.",
        "feat_precision": "<strong>Fractional MB Capacity Precision:</strong> Supports precise capacity inputs and saving, such as custom 180 KB or 0.18 MB volumes, for accurate packing.",
        "feat_json": "<strong>Offline JSON Indexing:</strong> Generates a standardized <code>index.json</code> directory catalog during output, listing file attributes for offline browsing.",
        "feat_semantic": "<strong>Rule-Based and Semantic Grouping:</strong> Group files together on the same medium using standard wildcards, regular expressions, or context-aware semantic clustering.",
        "commit_title": "Privacy &amp; Performance Commitments",
        "commit_p1": "Burn to the Brim is built with user privacy, system performance, and clean software standards in mind:",
        "commit_offline": "<strong>Strict Offline Execution (No Spyware):</strong> The application operates entirely locally on your system. It never logs, tracks, or transmits your directory paths, file names, or personal information over the network.",
        "commit_adware": "<strong>No Adware:</strong> The application does not contain advertisements, commercial banners, or sponsored popups.",
        "commit_footprint": "<strong>Ultra-Lightweight Footprint:</strong> Statically compiled native C++ code ensures minimal memory usage and zero framework overhead.",
        "commit_clutter": "<strong>Zero System Clutter:</strong> The installer registers only what is necessary for Windows integration, and the application does not litter your registry with junk files.",
        "commit_uninst": "<strong>Clean Uninstallation:</strong> Standard uninstall removal completely cleans all program directories and shortcuts from your system.",
        "copyright": "&copy; BTTB Team. All rights reserved.",
        "motto": "Archive intelligently"
    },
    "es": {
        "title": "Burn to the Brim: Resumen",
        "h1": "Resumen del Aplicativo",
        "subtitle": "Un optimizador de espacio de almacenamiento y utilidad de embalaje de directorios de alto rendimiento.",
        "desc_title": "Descripción del Producto",
        "desc_p1": "<strong>Burn to the Brim</strong> es una herramienta profesional de embalaje de archivos y optimización de almacenamiento. Resuelve el clásico desafío de optimización del problema del empaquetado (bin-packing) y la suma de subconjuntos, permitiéndole encajar archivos y carpetas perfectamente en soportes de almacenamiento\u2014incluyendo CDs, DVDs, Blu-rays, unidades flash USB o discos virtuales con capacidad personalizada\u2014con prácticamente cero desperdicio de espacio.",
        "desc_p2": "Utilizando un motor de resolución por backtracking recursivo altamente optimizado, la aplicación analiza las estructuras de directorios y los tamaños de archivos para calcular configuraciones de diseño casi perfectas en milisegundos. Con un motor multiproceso no bloqueante, la interfaz de usuario se mantiene completamente fluida e interactiva incluso al calcular soluciones para conjuntos de datos muy grandes.",
        "features_title": "Características Clave",
        "feat_span": "<strong>División Multivolumen:</strong> Divide automáticamente jerarquías de archivos masivas que superan las capacidades de un solo disco en carpetas de volúmenes de tamaño óptimo.",
        "feat_par3": "<strong>Paridad PAR3 Cauchy Reed-Solomon:</strong> Crea automáticamente archivos de paridad PAR3 para la corrección de errores, considerando el coste adicional en las comprobaciones de diseño.",
        "feat_verify": "<strong>Verificación de Integridad y Recuperación:</strong> Escanea y verifica volúmenes contra archivos de paridad para identificar elementos corruptos y realizar restauraciones no destructivas.",
        "feat_menu": "<strong>Integración con el Menú Contextual del Explorador:</strong> Agrega instantáneamente archivos y carpetas a sesiones activas mediante el menú del botón secundario de Windows Explorer.",
        "feat_cli": "<strong>Motor Híbrido GUI/CLI:</strong> Cambia sin problemas entre una interfaz de usuario nativa ligera (Win32 / GTK 4) y un modo de línea de comandos completo para scripting.",
        "feat_unicode": "<strong>Soporte Unicode y Rutas Largas:</strong> Totalmente compatible con codificaciones de caracteres internacionales y rutas largas UNC de Windows de hasta 32.767 caracteres.",
        "feat_render": "<strong>Renderizado de Árbol No Bloqueante:</strong> Muestra conjuntos de datos de soluciones de forma incremental utilizando los ciclos inactivos del bucle principal para evitar congelaciones de la GUI.",
        "feat_precision": "<strong>Precisión de Capacidad en MB Fraccionales:</strong> Permite entradas precisas de capacidad y guardado, como volúmenes personalizados de 180 KB o 0.18 MB, para un empaquetado exacto.",
        "feat_json": "<strong>Indexación JSON sin conexión:</strong> Genera un catálogo de directorios estandarizado <code>index.json</code> al procesar la salida, enumerando los atributos del archivo para navegación offline.",
        "feat_semantic": "<strong>Agrupación Semántica y por Reglas:</strong> Agrupa archivos en el mismo medio usando comodines estándar, expresiones regulares o agrupación semántica basada en el contexto.",
        "commit_title": "Compromiso de Privacidad y Rendimiento",
        "commit_p1": "Burn to the Brim está diseñado pensando en la privacidad del usuario, el rendimiento del sistema y los estándares de software limpio:",
        "commit_offline": "<strong>Ejecución Estricta Fuera de Línea (Sin Spyware):</strong> El aplicativo opera completamente a nivel local. Nunca registra, rastrea ni transmite por la red sus rutas de directorios, nombres de archivos o información personal.",
        "commit_adware": "<strong>Sin Adware:</strong> La aplicación no contiene anuncios publicitarios, banners comerciales ni ventanas emergentes patrocinadas.",
        "commit_footprint": "<strong>Consumo de Recursos Ultra Ligero:</strong> El código nativo en C++ compilado estáticamente asegura un uso mínimo de memoria y elimina cualquier sobrecarga de marcos de trabajo.",
        "commit_clutter": "<strong>Sin Basura en el Sistema:</strong> El instalador registra solo lo necesario para la integración en Windows, y la aplicación no ensucia su registro con archivos innecesarios.",
        "commit_uninst": "<strong>Desinstalación Limpia:</strong> El proceso de desinstalación estándar elimina por completo todos los directorios y accesos directos del programa de su sistema.",
        "copyright": "&copy; Equipo BTTB. Todos los derechos reservados.",
        "motto": "Archive inteligentemente"
    },
    "fr": {
        "title": "Burn to the Brim: Aperçu",
        "h1": "Aperçu de l'Application",
        "subtitle": "Un optimiseur d'espace de stockage de haute performance et utilitaire de regroupement de répertoires.",
        "desc_title": "Description du Produit",
        "desc_p1": "<strong>Burn to the Brim</strong> est un utilitaire professionnel de conditionnement de fichiers et d'optimisation du stockage. Il résout le défi classique d'optimisation du problème du sac à dos (bin-packing) et de la somme des sous-ensembles, vous permettant de faire tenir des fichiers et des dossiers parfaitement sur des supports de stockage cibles\u2014y compris les CD, DVD, Blu-ray enregistrables, les clés USB ou les disques virtuels à capacité personnalisée\u2014avec pratiquement aucun espace gaspillé.",
        "desc_p2": "Utilisant un solveur de backtracking récursif hautement optimisé, l'application analyse les structures de répertoires et les tailles de fichiers pour calculer des configurations de disposition presque parfaites en quelques millisecondes. Grâce à un moteur multithread non bloquant, l'interface utilisateur reste entièrement fluide et interactive, même lors du calcul de solutions pour de très grands ensembles de données.",
        "features_title": "Caractéristiques Clés",
        "feat_span": "<strong>Répartition Multivolume:</strong> Divise automatiquement les hiérarchies de fichiers massives qui dépassent les capacités d'un seul disque en dossiers de volumes de taille optimale.",
        "feat_par3": "<strong>Parité PAR3 Cauchy Reed-Solomon:</strong> Crée automatiquement des archives de parité PAR3 pour la correction d'erreurs, en intégrant directement la surcharge dans les vérifications de disposition.",
        "feat_verify": "<strong>Vérification d'Intégrité et Récupération:</strong> Analyse et vérifie les volumes par rapport aux fichiers de parité pour identifier les éléments corrompus et effectuer une restauration non destructive.",
        "feat_menu": "<strong>Intégration du Menu Contextuel de l'Explorateur:</strong> Ajoutez instantanément des fichiers et des dossiers aux sessions actives via le menu clic droit de Windows Explorer.",
        "feat_cli": "<strong>Moteur Hybride GUI/CLI:</strong> Basculez de manière transparente entre une interface utilisateur native légère (Win32 / GTK 4) et un mode ligne de commande complet pour les scripts.",
        "feat_unicode": "<strong>Support Unicode et Chemins Longs:</strong> Entièrement compatible avec les encodages de caractères internationaux et les chemins d'accès longs UNC de Windows jusqu'à 32 767 caractères.",
        "feat_render": "<strong>Rendu d'Arborescence Non Bloquant:</strong> Affiche de grands ensembles de données de solutions de manière incrémentielle en utilisant les cycles inactifs de la boucle principale pour éviter le blocage de la GUI.",
        "feat_precision": "<strong>Précision de Capacité en MB Fractionnaire:</strong> Prend en charge des entrées et des sauvegardes précises de capacité, comme des volumes personnalisés de 180 Ko ou 0.18 Mo, pour un conditionnement précis.",
        "feat_json": "<strong>Indexation JSON Hors Ligne:</strong> Génère un catalogue de répertoires standardisé <code>index.json</code> lors de la sortie, listant les attributs des fichiers pour une navigation offline.",
        "feat_semantic": "<strong>Regroupement Sémantique et par Règles:</strong> Regroupez des fichiers sur le même support en utilisant des jokers standard, des expressions régulières ou un clustering sémantique contextuel.",
        "commit_title": "Engagements de Confidentialité et de Performance",
        "commit_p1": "Burn to the Brim est conçu avec le respect de la vie privée de l'utilisateur, les performances du système et des normes de logiciels propres à l'esprit :",
        "commit_offline": "<strong>Exécution Hors Ligne Stricte (Aucun Logiciel Espion):</strong> L'application fonctionne entièrement localement. Elle n'enregistre, ne suit ni ne transmet jamais vos chemins de répertoires, noms de fichiers ou informations personnelles sur le réseau.",
        "commit_adware": "<strong>Aucun Adware:</strong> L'application ne contient pas de publicités, de bannières commerciales ou de fenêtres contextuelles sponsorisées.",
        "commit_footprint": "<strong>Empreinte Ultra-Légère:</strong> Le code C++ natif compilé statiquement assure une utilisation minimale de la mémoire et élimine tout coût de framework.",
        "commit_clutter": "<strong>Zéro Encombrement du Système:</strong> L'installateur n'enregistre que ce qui est nécessaire pour l'intégration de Windows, et l'application ne pollue pas votre registre de fichiers inutiles.",
        "commit_uninst": "<strong>Désinstallation Propre:</strong> La procédure de désinstallation standard supprime complètement tous les répertoires et raccourcis du programme de votre système.",
        "copyright": "&copy; Équipe BTTB. Tous droits réservés.",
        "motto": "Archivez intelligemment"
    },
    "nl": {
        "title": "Burn to the Brim: Overzicht",
        "h1": "Applicatieoverzicht",
        "subtitle": "Een hoogwaardige opslagruimte-optimalisatietool en hulpprogramma voor het inpakken van mappen.",
        "desc_title": "Productomschrijving",
        "desc_p1": "<strong>Burn to the Brim</strong> is een professioneel hulpprogramma voor het inpakken van bestanden en opslagoptimalisatie. Het lost het klassieke bin-packing en subset-sum optimalisatieprobleem op, waardoor u bestanden en mappen perfect kunt passen op doelopslagmedia\u2014inclusief beschrijfbare CD's, DVD's, Blu-rays, USB-flashstations of virtuele schijven met een aangepaste capaciteit\u2014met vrijwel nul verspilde ruimte.",
        "desc_p2": "Met behulp van een sterk geoptimaliseerde recursieve backtracking-solver analyseert de applicatie directory-structuren en bestandsgrootten om in milliseconden bijna perfecte lay-outconfiguraties te berekenen. Dankzij een niet-blokkerende multithreaded engine blijft de gebruikersinterface volledig vloeiend en interactief, zelfs bij het berekenen van oplossingen voor zeer grote datasets.",
        "features_title": "Belangrijkste Kenmerken",
        "feat_span": "<strong>Splitsen Over Meerdere Volumes:</strong> Breekt massieve bestandshiërarchieën die de capaciteit van één schijf overschrijden automatisch op in optimaal gedimensioneerde volumemappen.",
        "feat_par3": "<strong>Cauchy Reed-Solomon PAR3 Pariteit:</strong> Creëert automatisch PAR3 pariteitsarchieven voor foutcorrectie, waarbij overhead direct in lay-outcontroles wordt meegerekend.",
        "feat_verify": "<strong>Integriteitsverificatie &amp; Herstel:</strong> Scan en verifieer volumes tegen pariteitsbestanden om beschadigde items te identificeren en voer niet-destructief herstel uit.",
        "feat_menu": "<strong>Explorer Contextmenu Integratie:</strong> Voeg bestanden en mappen direct toe aan actieve sessies via het rechtermuisknopmenu van Windows Explorer.",
        "feat_cli": "<strong>Hybride GUI/CLI Engine:</strong> Schakel naadloos tussen een lichtgewicht native gebruikersinterface (Win32 / GTK 4) and een volledige opdrachtregelmodus voor scripting.",
        "feat_unicode": "<strong>Unicode &amp; Lange Pad Ondersteuning:</strong> Volledig compatibel met internationale karaktercoderingen en Windows UNC lange paden tot 32.767 karakters.",
        "feat_render": "<strong>Niet-Blokkerende Tree Rendering:</strong> Geeft grote resultaten-datasets incrementeel weer met behulp van de inactieve cycli van de hoofdlus om GUI-bevriezing te voorkomen.",
        "feat_precision": "<strong>Fractie MB Capaciteitsprecisie:</strong> Ondersteunt nauwkeurige capaciteitsinvoer en -besparing, zoals aangepaste 180 KB of 0.18 MB volumes, voor exact inpakken.",
        "feat_json": "<strong>Offline JSON-Indexering:</strong> Genereert tijdens de uitvoer een gestandaardiseerde <code>index.json</code> mappen-catalogus die bestandsattributen bevat voor offline browsen.",
        "feat_semantic": "<strong>Regelgebaseerde en Semantische Groepering:</strong> Groepeer bestanden samen op hetzelfde medium met behulp van standaard wildcards, reguliere expressies of contextbewuste semantische clustering.",
        "commit_title": "Privacy- en Prestatiegaranties",
        "commit_p1": "Burn to the Brim is gebouwd met de privacy van de gebruiker, systeemprestaties en schone softwarestandaarden in het achterhoofd:",
        "commit_offline": "<strong>Strikte Offline Uitvoering (Geen Spyware):</strong> De applicatie werkt volledig lokaal. Het registreert, volgt of verzendt uw directorypaden, bestandsnamen of persoonlijke gegevens nooit over het netwerk.",
        "commit_adware": "<strong>Geen Adware:</strong> De applicatie bevat geen advertenties, commerciële banners of gesponsorde pop-ups.",
        "commit_footprint": "<strong>Ultralichte Systeemvoetafdruk:</strong> Statisch gecompileerde native C++-code zorgt voor minimaal geheugengebruik en elimineert elke overhead van frameworks.",
        "commit_clutter": "<strong>Geen Systeemvervuiling:</strong> Het installatieprogramma registreert alleen wat nodig is voor Windows-integratie en de applicatie vervuilt uw register niet met onnodige bestanden.",
        "commit_uninst": "<strong>Schone Deïnstallatie:</strong> De standaard verwijderingsprocedure wist alle programmamappen en snelkoppelingen volledig van uw systeem.",
        "copyright": "&copy; BTTB Team. Alle rechten voorbehouden.",
        "motto": "Archiveer intelligent"
    },
    "de": {
        "title": "Burn to the Brim: \u00dcbersicht",
        "h1": "Anwendungs\u00fcbersicht",
        "subtitle": "Ein leistungsstarkes Tool zur Speicherplatzoptimierung und Verzeichnispackung.",
        "desc_title": "Produktbeschreibung",
        "desc_p1": "<strong>Burn to the Brim</strong> ist ein professionelles Dienstprogramm zur Dateiverpackung und Speicheroptimierung. Es l\u00f6st das klassische Beh\u00e4lterproblem (Bin-Packing) und das Teilmengensummenproblem (Subset-Sum) und erm\u00f6glicht es Ihnen, Dateien und Ordner perfekt auf Zielspeichermedien\u2014einschlie\u00dflich beschreibbarer CDs, DVDs, Blu-rays, USB-Sticks oder virtueller Laufwerke mit benutzerdefinierter Kapazit\u00e4t\u2014mit praktisch null Platzverschwendung zu verteilen.",
        "desc_p2": "Durch den Einsatz eines hochgradig optimierten rekursiven Backtracking-Solvers analysiert die Anwendung Verzeichnisstrukturen und Dateigr\u00f6\u00dfen, um in Millisekunden nahezu perfekte Layoutkonfigurationen zu berechnen. Dank einer nicht-blockierenden, multithreaded Engine bleibt die Benutzeroberfl\u00e4che auch bei der Berechnung von L\u00f6sungen f\u00fcr sehr gro\u00dfe Datens\u00e4tze v\u00f6llig fl\u00fcssig und interaktiv.",
        "features_title": "Hauptmerkmale",
        "feat_span": "<strong>Multi-Volume Spanning (Mehrteilige Archive):</strong> Teilt riesige Dateimengen, die die Kapazit\u00e4t eines einzelnen Datentr\u00e4gers \u00fcberschreiten, automatisch in optimal dimensionierte Bandordner auf.",
        "feat_par3": "<strong>Cauchy Reed-Solomon PAR3-Parit\u00e4t:</strong> Erstellt automatisch PAR3-Parit\u00e4tsarchive zur Fehlerkorrektur und ber\u00fccksichtigt den Overhead direkt bei den Solver-Pr\u00fcfungen.",
        "feat_verify": "<strong>Integrit\u00e4tspr\u00fcfung &amp; Wiederherstellung:</strong> Scant und verifiziert Volumes anhand von Parit\u00e4tsdateien, um besch\u00e4digte Elemente zu identifizieren, und f\u00fchrt eine zerst\u00f6rungsfreie, kopierbasierte Wiederherstellung durch.",
        "feat_menu": "<strong>Integration in das Explorer-Kontextmen\u00fc:</strong> Erm\u00f6glicht die Shell-Integration, um Dateien und Ordner \u00fcber das Windows Explorer-Rechtsklickmen\u00fc direkt an eine aktive Sitzung anzuh\u00e4ngen.",
        "feat_cli": "<strong>Hybride GUI/CLI-Engine:</strong> Wechselt nahtlos zwischen einer schlanken nativen Benutzeroberfl\u00e4che (Win32 / GTK 4) und einem vollwertigen Befehlszeilenmodus f\u00fcr Skripterstellung.",
        "feat_unicode": "<strong>Unterst\u00fctzung f\u00fcr Unicode &amp; Lange Pfade:</strong> Vollst\u00e4ndig kompatibel mit internationalen Zeichenkodierungen und Windows UNC-Pfaden von bis zu 32.767 Zeichen.",
        "feat_render": "<strong>Nicht-blockierendes Baum-Rendering:</strong> Zeigt gro\u00dfe Ergebnislisten inkrementell \u00fcber Leerlaufzyklen der Hauptschleife an, um ein Einfrieren der GUI zu verhindern.",
        "feat_precision": "<strong>Pr\u00e4zision bei fraktionellen MB-Kapazit\u00e4ten:</strong> Unterst\u00fctzt pr\u00e4zise Kapazit\u00e4tseingaben und -einsparungen, wie z. B. benutzerdefinierte 180-KB- oder 0,18-MB-Volumes, f\u00fcr exaktes Packen.",
        "feat_json": "<strong>Offline-JSON-Indizierung:</strong> Generiert w\u00e4hrend der Ausgabe einen standardisierten <code>index.json</code>-Verzeichniskatalog, der Dateiattribute f\u00fcr das Offline-Browsen auflistet.",
        "feat_semantic": "<strong>Regelbasierte und Semantische Gruppierung:</strong> Gruppiert Dateien auf demselben Medium mithilfe von Standard-Wildcards, regul\u00e4ren Ausdr\u00fccken oder kontextbezogenem semantischem Clustering.",
        "commit_title": "Datenschutz- und Performance-Garantien",
        "commit_p1": "Burn to the Brim wurde im Hinblick auf den Schutz der Benutzersph\u00e4re, Systemleistung und saubere Softwarestandards entwickelt:",
        "commit_offline": "<strong>Strikte Offline-Ausf\u00fchrung (Keine Spyware):</strong> Die Anwendung l\u00e4uft vollst\u00e4ndig lokal auf Ihrem System. Sie protokolliert, verfolgt oder \u00fcbertr\u00e4gt Ihre Verzeichnispfade, Dateinamen oder pers\u00f6nlichen Daten niemals \u00fcber das Netzwerk.",
        "commit_adware": "<strong>Keine Adware:</strong> Die Anwendung enth\u00e4lt keine Werbung, kommerzielle Banner oder gesponserte Popups.",
        "commit_footprint": "<strong>Ultraleichter Ressourcenbedarf:</strong> Statisch kompilierter nativer C++-Code sorgt f\u00fcr minimalen Speicherverbrauch und keinerlei Framework-Overhead.",
        "commit_clutter": "<strong>Kein Systemm\u00fcll:</strong> Das Installationsprogramm registriert nur das, was f\u00fcr die Windows-Integration erforderlich ist, und die Anwendung hinterl\u00e4sst keinen Datenm\u00fcll in Ihrer Registrierung.",
        "commit_uninst": "<strong>Saubere Deinstallation:</strong> Das Standard-Deinstallationsverfahren entfernt alle Programmverzeichnisse und Verkn\u00fchpfungen vollst\u00e4ndig von Ihrem System.",
        "copyright": "&copy; BTTB-Team. Alle Rechte vorbehalten.",
        "motto": "Archivieren Sie intelligent"
    },
    "zh": {
        "title": "Burn to the Brim: 概述",
        "h1": "应用程序概述",
        "subtitle": "高性能存储空间优化与目录打包工具。",
        "desc_title": "产品描述",
        "desc_p1": "<strong>Burn to the Brim</strong> 是一款专业的文件打包与存储优化工具。它完美解决了经典的装箱问题（bin-packing）和子集和优化挑战，允许您将文件和文件夹完美地放入目标存储介质中——包括可刻录的 CD、DVD、蓝光光盘、USB 闪存驱动器或自定义容量的虚拟驱动器，几乎达到零空间浪费。",
        "desc_p2": "该程序采用高度优化的递归回溯求解器，可在毫秒内分析目录结构和文件大小，以计算出几近完美的布局配置。配合非阻塞多线程引擎，即使在为极大数据集计算解决方案时，用户界面也能保持完全流畅与交互性。",
        "features_title": "主要功能",
        "feat_span": "<strong>多卷分发:</strong> 自动将超过单个磁盘容量的海量文件层级分割到大小最优的卷文件夹中。",
        "feat_par3": "<strong>Cauchy Reed-Solomon PAR3 冗余校验:</strong> 自动创建用于错误纠正的 PAR3 冗余归档，并将开销直接计入布局检查中。",
        "feat_verify": "<strong>完整性校验与恢复:</strong> 扫描并对比校验文件以识别损坏的项目，执行无损、基于复制的安全恢复。",
        "feat_menu": "<strong>资源管理器右键菜单集成:</strong> 通过 Windows 资源管理器右键菜单瞬间将文件和文件夹直接附加到活动会话中。",
        "feat_cli": "<strong>混合 GUI/CLI 引擎:</strong> 在轻量级原生用户界面（Win32 / GTK 4）和用于脚本编写的完整命令行模式之间无缝切换。",
        "feat_unicode": "<strong>Unicode 和长路径支持:</strong> 完全兼容国际字符编码和高达 32,767 个字符的 Windows UNC 长路径。",
        "feat_render": "<strong>非阻塞树状渲染:</strong> 利用主循环空闲周期增量显示大型解决方案数据集，防止 GUI 冻结。",
        "feat_precision": "<strong>MB 小数级容量精度:</strong> 支持精确的容量输入与保存，如自定义 180 KB 或 0.18 MB 卷，以实现精确打包。",
        "feat_json": "<strong>离线 JSON 索引:</strong> 在输出期间生成标准化的 <code>index.json</code> 目录清单，列出文件属性以便离线浏览。",
        "feat_semantic": "<strong>基于规则和语义的分组:</strong> 使用标准通配符、正则表达式或上下文感知的语义聚类，将相关文件归类打包在同一介质中。",
        "commit_title": "隐私与性能承诺",
        "commit_p1": "Burn to the Brim 的设计宗旨是保障用户隐私、系统性能和干净的软件标准：",
        "commit_offline": "<strong>严格的离线运行（无间谍软件）:</strong> 应用程序完全在您系统本地运行。它绝不会记录、跟踪或通过网络传输您的目录路径、文件名或任何个人信息。",
        "commit_adware": "<strong>无广告软件:</strong> 本程序不包含任何广告、商业横幅或赞助弹窗。",
        "commit_footprint": "<strong>极轻量级的资源占用:</strong> 静态编译的原生 C++ 代码确保极低的内存消耗，且无任何框架开销。",
        "commit_clutter": "<strong>零系统杂乱:</strong> 安装程序仅注册 Windows 集成所必需的内容，程序不会在您的注册表中留下任何垃圾文件。",
        "commit_uninst": "<strong>干净卸载:</strong> 标准的卸载程序将从您的系统中彻底清除所有的程序目录和快捷方式。",
        "copyright": "&copy; BTTB 团队。保留所有权利。",
        "motto": "智能归档"
    },
    "ja": {
        "title": "Burn to the Brim: 概要",
        "h1": "アプリケーションの概要",
        "subtitle": "高性能なストレージ容量最適化およびディレクトリパッキングユーティリティ。",
        "desc_title": "製品説明",
        "desc_p1": "<strong>Burn to the Brim</strong> は、プロフェッショナルなファイルパッキングおよびストレージ最適化ユーティリティです。古典的なビンパッキング問題と部分和問題の最適化チャレンジを解決し、ファイルやフォルダを書き込み可能なCD、DVD、ブルーレイ、USBメモリ、またはカスタム容量の仮想ドライブなどのターゲットストレージメディアに、無駄なスペースをほぼゼロにして完璧に収めることができます。",
        "desc_p2": "高度に最適化された再帰的バックトラッキングソルバーを採用し、アプリケーションはディレクトリ構造とファイルサイズを分析して、ほぼ完璧なレイアウト構成をミリ秒単位で計算します。非ブロッキングのマルチスレッドエンジンにより、非常に大規模なデータセットのソリューションを計算している間でも、ユーザーインターフェースは完全に滑らかでインタラクティブな状態を維持します。",
        "features_title": "主な機能",
        "feat_span": "<strong>マルチボリュームスパンニング:</strong> シングルドライブの容量を超える膨大なファイル階層を、最適なサイズに分割して自動的にボリュームフォルダに格納します。",
        "feat_par3": "<strong>Cauchy Reed-Solomon PAR3パリティ:</strong> エラー訂正用のPAR3パリティアーカイブを自動生成し、オーバーヘッドをレイアウトチェックに直接反映させます。",
        "feat_verify": "<strong>整合性検証とデータ修復:</strong> パリティファイルと照合してボリュームをスキャン・検証し、破損したアイテムを特定して、複製による非破壊的な復元を実行します。",
        "feat_menu": "<strong>Explorerコンテキストメニュー統合:</strong> Windows Explorerの右クリックメニューを介して、アクティブなセッションにファイルやフォルダを直接瞬時に追加します。",
        "feat_cli": "<strong>ハイブリッドGUI/CLIエンジン:</strong> 軽量なネイティブUI（Win32 / GTK 4）と、スクリプト作成用の完全なコマンドラインモードをシームレスに切り替えます。",
        "feat_unicode": "<strong>Unicodeおよび長パスのサポート:</strong> 国際文字エンコーディングおよび最大32,767文字のWindows UNC長パスと完全に互換性があります。",
        "feat_render": "<strong>非ブロッキングツリー表示:</strong> メインループのアイドルサイクルを利用して結果をインクリメンタルに描画し、GUIのフリーズを防ぎます。",
        "feat_precision": "<strong>小数点MB単位の容量精度:</strong> カスタムの180 KBや0.18 MBなど、正確なパッキングのために微細な容量入力と保存をサポートします。",
        "feat_json": "<strong>オフラインJSONインデックス作成:</strong> 出力時に標準化された <code>index.json</code> ディレクトリカタログを生成し、オフライン閲覧用にファイル属性をリストします。",
        "feat_semantic": "<strong>ルールベースと意味的グループ化:</strong> 標準のワイルドカード、正規表現、または文脈を考慮した意味的クラスタリングを使用して、ファイルを同じメディアにまとめます。",
        "commit_title": "プライバシーとパフォーマンスの約束",
        "commit_p1": "Burn to the Brimは、ユーザーのプライバシー保護、システム性能の最大化、クリーンなソフトウェア基準を念頭に置いて構築されています：",
        "commit_offline": "<strong>厳格なオフライン実行（スパイウェアなし）:</strong> アプリケーションは完全にローカルで動作します。ディレクトリパス、ファイル名、または個人情報をネットワーク経由で記録、追跡、送信することは一切ありません。",
        "commit_adware": "<strong>アドウェアなし:</strong> アプリケーションに広告、商業バナー、またはスポンサーポップアップは含まれていません。",
        "commit_footprint": "<strong>超軽量フットプリント:</strong> 静的にコンパイルされたネイティブC++コードにより、メモリ使用量を最小限に抑え、フレームワークのオーバーヘッドをゼロにします。",
        "commit_clutter": "<strong>システムを汚さない設計:</strong> インストーラーはWindows統合に必要な設定のみを登録し、アプリケーションが不要なレジストリエントリを残すことはありません。",
        "commit_uninst": "<strong>クリーンなアンインストール:</strong> 標準のアンインストール手順により、すべてのプログラムディレクトリとショートカットがシステムから完全に削除されます。",
        "copyright": "&copy; BTTBチーム. All rights reserved.",
        "motto": "インテリジェントにアーカイブ"
    },
    "it": {
        "title": "Burn to the Brim: Panoramica",
        "h1": "Panoramica dell'Applicazione",
        "subtitle": "Un ottimizzatore dello spazio di archiviazione e utilit\u00e0 di compressione directory ad alte prestazioni.",
        "desc_title": "Descrizione del Prodotto",
        "desc_p1": "<strong>Burn to the Brim</strong> \u00e8 un'utilit\u00e0 professionale di confezionamento file e ottimizzazione dello spazio di archiviazione. Risolve la classica sfida del bin-packing e Subset-Sum, consentendo di adattare perfettamente file e cartelle sui supporti di memoria di destinazione\u2014inclusi CD, DVD, Blu-ray registrabili, chiavette USB o unità virtuali a capacità personalizzata\u2014con uno spreco di spazio praticamente pari a zero.",
        "desc_p2": "Utilizzando un risolutore ricorsivo con backtracking altamente ottimizzato, l'applicazione analizza le strutture delle directory e le dimensioni dei file per calcolare configurazioni di layout quasi perfette in millisecondi. Grazie a un motore multithread non bloccante, l'interfaccia utente rimane completamente fluida e interattiva anche durante il calcolo delle soluzioni per set di dati molto grandi.",
        "features_title": "Caratteristiche Principali",
        "feat_span": "<strong>Suddivisione Multivolume:</strong> Divide automaticamente gerarchie di file massicce che superano le capacità di una singola unità in cartelle di volumi di dimensioni ottimali.",
        "feat_par3": "<strong>Parit\u00e0 PAR3 Cauchy Reed-Solomon:</strong> Crea automaticamente archivi di parità PAR3 per la correzione degli errori, considerando il sovraccarico direttamente nei controlli di layout.",
        "feat_verify": "<strong>Verifica dell'Integrit\u00e0 e Ripristino:</strong> Scansiona e verifica i volumi rispetto ai file di parità per identificare gli elementi corrotti ed eseguire un ripristino non distruttivo basato sulla copia.",
        "feat_menu": "<strong>Integrazione con il Menu Contestuale di Explorer:</strong> Aggiunge istantaneamente file e cartelle alle sessioni attive tramite il menu contestuale del pulsante destro del mouse di Windows Explorer.",
        "feat_cli": "<strong>Motore Ibrido GUI/CLI:</strong> Passa in modo trasparente da un'interfaccia utente nativa leggera (Win32 / GTK 4) a una modalità a riga di comando completa per lo scripting.",
        "feat_unicode": "<strong>Supporto Unicode e Percorsi Lunghi:</strong> Completamente compatibile con le codifiche dei caratteri internazionali e i percorsi lunghi UNC di Windows fino a 32.767 caratteri.",
        "feat_render": "<strong>Rendering ad Albero Non Bloccante:</strong> Visualizza grandi set di dati delle soluzioni in modo incrementale utilizzando i cicli inattivi del ciclo principale per evitare il congelamento della GUI.",
        "feat_precision": "<strong>Precisione di Capacit\u00e0 in MB Frazionari:</strong> Supporta input precisi di capacità e salvataggi, come volumi personalizzati da 180 KB o 0.18 MB, per un confezionamento esatto.",
        "feat_json": "<strong>Indicizzazione JSON Offline:</strong> Genera un catalogo di directory standardizzato <code>index.json</code> durante l'output, elencando gli attributi dei file per la navigazione offline.",
        "feat_semantic": "<strong>Raggruppamento Semantico e per Regole:</strong> Raggruppa i file sullo stesso supporto utilizzando caratteri jolly standard, espressioni regolari o clustering semantico sensibile al contesto.",
        "commit_title": "Impegni di Riservatezza e Prestazioni",
        "commit_p1": "Burn to the Brim \u00e8 progettato tenendo conto della privacy dell'utente, delle prestazioni del sistema e di standard di software pulito:",
        "commit_offline": "<strong>Esecuzione Rigorosamente Offline (No Spyware):</strong> L'applicazione funziona interamente in locale. Non registra, traccia o trasmette mai i percorsi delle directory, i nomi dei file o le informazioni personali sulla rete.",
        "commit_adware": "<strong>Nessun Adware:</strong> L'applicazione non contiene annunci pubblicitari, banner commerciali o popup sponsorizzati.",
        "commit_footprint": "<strong>Impronta Ultra-Leggera:</strong> Il codice C++ nativo compilato staticamente garantisce un utilizzo minimo della memoria e zero overhead del framework.",
        "commit_clutter": "<strong>Zero Disordine nel Sistema:</strong> Il programma di installazione registra solo il necessario per l'integrazione con Windows e l'applicazione non intasa il registro con file spazzatura.",
        "commit_uninst": "<strong>Disinstallazione Pulita:</strong> La procedura di rimozione standard elimina completamente tutte le directory del programma e i collegamenti dal sistema.",
        "copyright": "&copy; Team BTTB. Tutti i diritti riservati.",
        "motto": "Archivia con intelligenza"
    },
    "el": {
        "title": "Burn to the Brim: \u0395\u03c0\u03b9\u03c3\u03ba\u03cc\u03c0\u03b7\u03c3\u03b7",
        "h1": "\u0395\u03c0\u03b9\u03c3\u03ba\u03cc\u03c0\u03b7\u03c3\u03b7 \u0395\u03c6\u03b1\u03c1\u03bc\u03bf\u03b3\u03ae\u03c2",
        "subtitle": "\u0384\u03bd\u03b1 \u03b5\u03c1\u03b3\u03b1\u03bb\u03b5\u03af\u03bf \u03b2\u03b5\u03bb\u03c4\u03b9\u03c3\u03c4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7\u03c2 \u03c7\u03ce\u03c1\u03bf\u03c5 \u03b1\u03c0\u03bf\u03b8\u03ae\u03ba\u03b5\u03c5\u03c3\u03b7\u03c2 \u03ba\u03b1\u03b9 \u03c3\u03c5\u03c3\u03ba\u03b5\u03c5\u03b1\u03c3\u03af\u03b1\u03c2 \u03ba\u03b1\u03c4\u03b1\u03bb\u03cc\u03b3\u03c9\u03bd \u03c5\u03c8\u03b7\u03bb\u03ae\u03c2 \u03b1\u03c0\u03cc\u03b4\u03bf\u03c3\u03b7\u03c2.",
        "desc_title": "\u03a0\u03b5\u03c1\u03b9\u03b3\u03c1\u03b1\u03c6\u03ae \u03a0\u03c1\u03bf\u03ca\u03cc\u03bd\u03c4\u03bf\u03c2",
        "desc_p1": "\u03c4\u03bf <strong>Burn to the Brim</strong> \u03b5\u03af\u03bd\u03b1\u03b9 \u03bc\u03b9\u03b1 \u03b5\u03c0\u03b1\u03b3\u03b3\u03b5\u03bb\u03bc\u03b1\u03c4\u03b9\u03ba\u03ae \u03b5\u03c6\u03b1\u03c1\u03bc\u03bf\u03b3\u03ae \u03c3\u03c5\u03c3\u03ba\u03b5\u03c5\u03b1\u03c3\u03af\u03b1\u03c2 \u03b1\u03c1\u03c7\u03b5\u03af\u03c9\u03bd \u03ba\u03b1\u03b9 \u03b2\u03b5\u03bb\u03c4\u03b9\u03c3\u03c4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7\u03c2 \u03b1\u03c0\u03bf\u03b8\u03ae\u03ba\u03b5\u03c5\u03c3\u03b7\u03c2. \u0395\u03c0\u03b9\u03bb\u03cd\u03b5\u03b9 \u03c4\u03bf \u03ba\u03bb\u03b1\u03c3\u03b9\u03ba\u03cc \u03c0\u03c1\u03cc\u03b2\u03bb\u03b7\u03bc\u03b1 \u03c3\u03c5\u03c3\u03ba\u03b5\u03c5\u03b1\u03c3\u03af\u03b1\u03c2 (bin-packing) \u03ba\u03b1\u03b9 subset-sum, \u03b5\u03c0\u03b9\u03c4\u03c1\u03ad\u03c0\u03bf\u03bd\u03c4\u03ac\u03c2 \u03c3\u03b1\u03c2 \u03bd\u03b1 \u03c4\u03bf\u03c0\u03bf\u03b8\u03b5\u03c4\u03ae\u03c3\u03b5\u03c4\u03b5 \u03b1\u03c1\u03c7\u03b5\u03af\u03b1 \u03ba\u03b1\u03b9 \u03c6\u03b1\u03ba\u03ad\u03bb\u03bf\u03c5\u03c2 \u03c4\u03ad\u03bb\u03b5\u03b9\u03b1 \u03c3\u03b5 \u03bc\u03ad\u03c3\u03b1 \u03b1\u03c0\u03bf\u03b8\u03ae\u03ba\u03b5\u03c5\u03c3\u03b7\u03c2 \u03c3\u03c4\u03cc\u03c7\u03bf\u03c5\u03c2\u2014\u03c3\u03c5\u03bc\u03c0\u03b5\u03c1\u03b9\u03bb\u03b1\u03bc\u03b2\u03b1\u03bd\u03bf\u03bc\u03ad\u03bd\u03c9\u03bd \u03b5\u03b3\u03b3\u03c1\u03ac\u03c8\u03b9\u03bc\u03c9\u03bd CD, DVD, Blu-ray, USB flash drives \u03ae \u03b5\u03b9\u03ba\u03bf\u03bd\u03b9\u03ba\u03ce\u03bd \u03b4\u03af\u03c3\u03ba\u03c9\u03bd\u2014\u03bc\u03b5 \u03c3\u03c7\u03b5\u03b4\u03cc\u03bd \u03bc\u03b7\u03b4\u03b5\u03bd\u03b9\u03ba\u03ae \u03b1\u03c0\u03ce\u03bb\u03b5\u03b9\u03b1 \u03c7\u03ce\u03c1\u03bf\u03c5.",
        "desc_p2": "\u03a7\u03c1\u03b7\u03c3\u03b9\u03bc\u03bf\u03c0\u03bf\u03b9\u03ce\u03bd\u03c4\u03b1\u03c2 \u03ad\u03bd\u03b1\u03bd \u03b5\u03be\u03b1\u03b9\u03c1\u03b5\u03c4\u03b9\u03ba\u03ac \u03b2\u03b5\u03bb\u03c4\u03b9\u03c3\u03c4\u03bf\u03c0\u03bf\u03b9\u03b7\u03bc\u03ad\u03bd\u03bf \u03b1\u03bd\u03b1\u03b4\u03c1\u03bf\u03bc\u03b9\u03ba\u03cc \u03b5\u03c0\u03b9\u03bb\u03cd\u03c4\u03b7 \u03bf\u03c0\u03b9\u03c3\u03b8\u03bf\u03b4\u03c1\u03cc\u03bc\u03b7\u03c3\u03b7\u03c2 (backtracking solver), \u03b7 \u03b5\u03c6\u03b1\u03c1\u03bc\u03bf\u03b3\u03ae \u03b1\u03bd\u03b1\u03bb\u03cd\u03b5\u03b9 \u03c4\u03b9\u03c2 \u03b4\u03bf\u03bc\u03ad\u03c2 \u03ba\u03b1\u03c4\u03b1\u03bb\u03cc\u03b3\u03c9\u03bd \u03ba\u03b1\u03b9 \u03c4\u03b1 \u03bc\u03b5\u03b3\u03ad\u03b8\u03b7 \u03c4\u03c9\u03bd \u03b1\u03c1\u03c7\u03b5\u03af\u03c9\u03bd \u03b3\u03b9\u03b1 \u03bd\u03b1 \u03c5\u03c0\u03bf\u03bb\u03bf\u03b3\u03af\u03c3\u03b5\u03b9 \u03c3\u03c7\u03b5\u03b4\u03cc\u03bd \u03c4\u03ad\u03bb\u03b5\u03b9\u03b5\u03c2 \u03b4\u03b9\u03b1\u03c4\u03ac\u03be\u03b5\u03b9\u03c2 \u03c3\u03b5 \u03c7\u03b9\u03bb\u03b9\u03bf\u03c3\u03c4\u03ac \u03c4\u03bf\u03c5 \u03b4\u03b5\u03c5\u03c4\u03b5\u03c1\u03bf\u03bb\u03ad\u03c0\u03c4\u03bf\u03c5. \u039c\u03b5 \u03bc\u03b9\u03b1 \u03bc\u03b7 \u03b1\u03c0\u03bf\u03ba\u03bb\u03b5\u03b9\u03c3\u03c4\u03b9\u03ba\u03ae \u03bc\u03b7\u03c7\u03b1\u03bd\u03ae \u03c0\u03bf\u03bb\u03bb\u03b1\u03c0\u03bb\u03ce\u03bd \u03bd\u03b7\u03bc\u03ac\u03c4\u03c9\u03bd, \u03b7 \u03b4\u03b9\u03b5\u03c0\u03b1\u03c6\u03ae \u03c7\u03c1\u03ae\u03c3\u03c4\u03b7 \u03c0\u03b1\u03c1\u03b1\u03bc\u03ad\u03bd\u03b5\u03b9 \u03c0\u03bb\u03ae\u03c1\u03c9\u03c2 \u03c1\u03b5\u03c5\u03c3\u03c4\u03ae \u03ba\u03b1\u03b9 \u03b4\u03b9\u03b1\u03b4\u03c1\u03b1\u03c3\u03c4\u03b9\u03ba\u03ae \u03b1\u03ba\u03cc\u03bc\u03b7 \u03ba\u03b1\u03b9 \u03ba\u03b1\u03c4\u03ac \u03c4\u03bf\u03bd \u03c5\u03c0\u03bf\u03bb\u03bf\u03b3\u03b9\u03c3\u03bc\u03cc \u03bb\u03cd\u03c3\u03b5\u03c9\u03bd \u03b3\u03b9\u03b1 \u03c0\u03bf\u03bb\u03cd \u03bc\u03b5\u03b3\u03ac\u03bb\u03b1 \u03c3\u03cd\u03bd\u03bf\u03bb\u03b1 \u03b4\u03b5\u03b4\u03bf\u03bc\u03ad\u03bd\u03c9\u03bd.",
        "features_title": "\u039a\u03cd\u03c1\u03b9\u03b1 \u03a7\u03b1\u03c1\u03b1\u03ba\u03c4\u03b7\u03c1\u03b9\u03c3\u03c4\u03b9\u03ba\u03ac",
        "feat_span": "<strong>\u039a\u03b1\u03c4\u03b1\u03bd\u03bf\u03bc\u03ae \u03c3\u03b5 \u03c0\u03bf\u03bb\u03bb\u03bf\u03cd\u03c2 \u03c4\u03cc\u03bc\u03bf\u03c5\u03c2:</strong> \u0394\u03b9\u03b1\u03c7\u03c9\u03c1\u03af\u03b6\u03b5\u03b9 \u03b1\u03c5\u03c4\u03cc\u03bc\u03b1\u03c4\u03b1 \u03c4\u03b5\u03c1\u03ac\u03c3\u03c4\u03b9\u03b5\u03c2 \u03b9\u03b5\u03c1\u03b1\u03c1\u03c7\u03af\u03b5\u03c2 \u03b1\u03c1\u03c7\u03b5\u03af\u03c9\u03bd \u03c0\u03bf\u03c5 \u03c5\u03c0\u03b5\u03c1\u03b2\u03b1\u03af\u03bd\u03bf\u03c5\u03bd \u03c4\u03b7 \u03c7\u03c9\u03c1\u03b7\u03c4\u03b9\u03ba\u03cc\u03c4\u03b7\u03c4\u03b1 \u03b5\u03bd\u03cc\u03c2 \u03b4\u03af\u03c3\u03ba\u03bf\u03c5 \u03c3\u03b5 \u03b2\u03ad\u03bb\u03c4\u03b9\u03c3\u03c4\u03b1 \u03bc\u03b5\u03b3\u03ad\u03b8\u03b7 \u03c6\u03b1\u03ba\u03ad\u03bb\u03c9\u03bd.",
        "feat_par3": "<strong>\u0399\u03c3\u03bf\u03c4\u03b9\u03bc\u03af\u03b1 PAR3 Cauchy Reed-Solomon:</strong> \u0394\u03b7\u03bc\u03b9\u03bf\u03c5\u03c1\u03b3\u03b5\u03af \u03b1\u03c5\u03c4\u03cc\u03bc\u03b1\u03c4\u03b1 \u03b1\u03c1\u03c7\u03b5\u03af\u03b1 \u03b9\u03c3\u03bf\u03c4\u03b9\u03bc\u03af\u03b1\u03c2 PAR3 \u03b3\u03b9\u03b1 \u03b4\u03b9\u03cc\u03c1\u03b8\u03c9\u03c3\u03b7 \u03c3\u03c6\u03b1\u03bb\u03bc\u03ac\u03c4\u03c9\u03bd.",
        "feat_verify": "<strong>\u0395\u03c0\u03b1\u03bb\u03ae\u03b8\u03b5\u03c5\u03c3\u03b7 \u0391\u03ba\u03b5\u03c1\u03b1\u03b9\u03cc\u03c4\u03b7\u03c4\u03b1\u03c2 &amp; \u0391\u03bd\u03ac\u03ba\u03c4\u03b7\u03c3\u03b7:</strong> \u03a3\u03b1\u03c1\u03ce\u03bd\u03b5\u03b9 \u03ba\u03b1\u03b9 \u03b5\u03c0\u03b1\u03bb\u03b7\u03b8\u03b5\u03cd\u03b5\u03b9 \u03c4\u03cc\u03bc\u03bf\u03c5\u03c2 \u03bc\u03b5 \u03b1\u03c1\u03c7\u03b5\u03af\u03b1 \u03b9\u03c3\u03bf\u03c4\u03b9\u03bc\u03af\u03b1\u03c2 \u03b3\u03b9\u03b1 \u03c4\u03bf\u03bd \u03b5\u03bd\u03c4\u03bf\u03c0\u03b9\u03c3\u03bc\u03cc \u03c6\u03b8\u03b1\u03c1\u03bc\u03ad\u03bd\u03c9\u03bd \u03c3\u03c4\u03bf\u03b9\u03c7\u03b5\u03af\u03c9\u03bd \u03ba\u03b1\u03b9 \u03b5\u03ba\u03c4\u03ad\u03bb\u03b5\u03c3\u03b7 \u03b1\u03bd\u03ac\u03ba\u03c4\u03b7\u03c3\u03b7\u03c2.",
        "feat_menu": "<strong>\u0395\u03bd\u03c3\u03c9\u03bc\u03ac\u03c4\u03c9\u03c3\u03b7 \u03c3\u03c4\u03bf \u039c\u03b5\u03bd\u03bf\u03cd \u03a0\u03b5\u03c1\u03b9\u03b5\u03c7\u03bf\u03bc\u03ad\u03bd\u03bf\u03c5:</strong> \u03a0\u03c1\u03bf\u03c3\u03b8\u03ad\u03c4\u03b5\u03b9 \u03b1\u03bc\u03ad\u03c3\u03c9\u03c2 \u03b1\u03c1\u03c7\u03b5\u03af\u03b1 \u03ba\u03b1\u03b9 \u03c6\u03b1\u03ba\u03ad\u03bb\u03bf\u03c5\u03c2 \u03bc\u03ad\u03c3\u03c9 \u03c4\u03bf\u03c5 \u03b4\u03b5\u03be\u03b9\u03bf\u03cd \u03ba\u03bb\u03b9\u03ba \u03c4\u03bf\u03c5 Windows Explorer.",
        "feat_cli": "<strong>\u03a5\u03b2\u03c1\u03b9\u03b4\u03b9\u03ba\u03ae \u039c\u03b7\u03c7\u03b1\u03bd\u03ae GUI/CLI:</strong> \u039c\u03b5\u03c4\u03ac\u03b2\u03b1\u03c3\u03b7 \u03c7\u03c9\u03c1\u03af\u03c2 \u03c0\u03c1\u03bf\u03b2\u03bb\u03ae\u03bc\u03b1\u03c4\u03b1 \u03bc\u03b5\u03c4\u03b1\u03be\u03cd \u03b5\u03bb\u03b1\u03c6\u03c1\u03bf\u03cd GUI (Win32 / GTK 4) \u03ba\u03b1\u03b9 \u03bb\u03b5\u03b9\u03c4\u03bf\u03c5\u03c1\u03b3\u03af\u03b1\u03c2 \u03b3\u03c1\u03b1\u03bc\u03bc\u03ae\u03c2 \u03b5\u03bd\u03c4\u03bf\u03bb\u03ce\u03bd \u03b3\u03b9\u03b1 \u03c3\u03b5\u03bd\u03ac\u03c1\u03b9\u03b1.",
        "feat_unicode": "<strong>\u03a5\u03c0\u03bf\u03c3\u03c4\u03ae\u03c1\u03b9\u03be\u03b7 Unicode &amp; \u039c\u03b5\u03b3\u03ac\u03bb\u03c9\u03bd \u0394\u03b9\u03b1\u03b4\u03c1\u03bf\u03bc\u03ce\u03bd:</strong> \u03a0\u03bb\u03ae\u03c1\u03c9\u03c2 \u03c3\u03c5\u03bc\u03b2\u03b1\u03c4\u03cc \u03bc\u03b5 \u03b4\u03b9\u03b5\u03b8\u03bd\u03b5\u03af\u03c2 \u03ba\u03c9\u03b4\u03b9\u03ba\u03bf\u03c0\u03bf\u03b9\u03ae\u03c3\u03b5\u03b9\u03c2 \u03c7\u03b1\u03c1\u03b1\u03ba\u03c4\u03ae\u03c1\u03c9\u03bd \u03ba\u03b1\u03b9 \u03bc\u03b5\u03b3\u03ac\u03bb\u03b1 \u03bc\u03bf\u03bd\u03bf\u03c0\u03ac\u03c4\u03b9\u03b1 Windows UNC \u03ad\u03c9\u03c2 32.767 \u03c7\u03b1\u03c1\u03b1\u03ba\u03c4\u03ae\u03c1\u03b5\u03c2.",
        "feat_render": "<strong>\u039c\u03b7 \u03b1\u03c0\u03bf\u03ba\u03bb\u03b5\u03b9\u03c3\u03c4\u03b9\u03ba\u03ae \u03a1\u03bf\u03ae \u0394\u03ad\u03bd\u03c4\u03c1\u03bf\u03c5:</strong> \u0395\u03bc\u03c6\u03b1\u03bd\u03af\u03b6\u03b5\u03b9 \u03bc\u03b5\u03b3\u03ac\u03bb\u03b1 \u03c3\u03cd\u03bd\u03bf\u03bb\u03b1 \u03b4\u03b5\u03b4\u03bf\u03bc\u03ad\u03bd\u03c9\u03bd \u03bb\u03cd\u03c3\u03b7\u03c2 \u03c3\u03c4\u03b1\u03b4\u03b9\u03b1\u03ba\u03ac, \u03b1\u03c0\u03bf\u03c6\u03b5\u03cd\u03b3\u03bf\u03bd\u03c4\u03b1\u03c2 \u03c4\u03bf \u03c0\u03ac\u03b3\u03c9\u03bc\u03b1 \u03c4\u03bf\u03c5 GUI.",
        "feat_precision": "<strong>\u0391\u03ba\u03c1\u03af\u03b2\u03b5\u03b9\u03b1 \u03a7\u03c9\u03c1\u03b7\u03c4\u03b9\u03ba\u03cc\u03c4\u03b7\u03c4\u03b1\u03c2 \u03c3\u03b5 \u039a\u03bb\u03b1\u03c3\u03bc\u03b1\u03c4\u03b9\u03ba\u03ac MB:</strong> \u03a5\u03c0\u03bf\u03c3\u03c4\u03b7\u03c1\u03af\u03b6\u03b5\u03b9 \u03b1\u03ba\u03c1\u03b9\u03b2\u03ae \u03b5\u03b9\u03c3\u03b1\u03b3\u03c9\u03b3\u03ae \u03c7\u03c9\u03c1\u03b7\u03c4\u03b9\u03ba\u03cc\u03c4\u03b7\u03c4\u03b1\u03c2 \u03ba\u03b1\u03b9 \u03b1\u03c0\u03bf\u03b8\u03ae\u03ba\u03b5\u03c5\u03c3\u03b7\u03c2, \u03cc\u03c0\u03c9\u03c2 \u03c0\u03c1\u03bf\u03c3\u03b1\u03c1\u03bc\u03bf\u03c3\u03bc\u03ad\u03bd\u03bf\u03c5\u03c2 \u03c4\u03cc\u03bc\u03bf\u03c5\u03c2 180 KB \u03ae 0.18 MB.",
        "feat_json": "<strong>\u039f\u03c6\u03bb\u03ac\u03b9\u03bd \u0395\u03c5\u03c1\u03b5\u03c4\u03b7\u03c1\u03af\u03b1\u03c3\u03b7 JSON:</strong> \u0394\u03b7\u03bc\u03b9\u03bf\u03c5\u03c1\u03b3\u03b5\u03af \u03ad\u03bd\u03b1\u03bd \u03c4\u03c5\u03c0\u03bf\u03c0\u03bf\u03b9\u03b7\u03bc\u03ad\u03bd\u03bf \u03ba\u03b1\u03c4\u03ac\u03bb\u03bf\u03b3\u03bf <code>index.json</code>, \u03ba\u03b1\u03c4\u03b1\u03b3\u03c1\u03ac\u03c6\u03bf\u03bd\u03c4\u03b1\u03c2 \u03c4\u03b1 \u03c7\u03b1\u03c1\u03b1\u03ba\u03c4\u03b7\u03c1\u03b9\u03c3\u03c4\u03b9\u03ba\u03ac \u03c4\u03c9\u03bd \u03b1\u03c1\u03c7\u03b5\u03af\u03c9\u03bd.",
        "feat_semantic": "<strong>\u0392\u03b1\u03c3\u03b9\u03c3\u03bc\u03ad\u03bd\u03b7 \u03c3\u03b5 \u03ba\u03b1\u03bd\u03cc\u03bd\u03b5\u03c2 \u03ba\u03b1\u03b9 \u03c3\u03b7\u03bc\u03b1\u03c3\u03b9\u03bf\u03bb\u03bf\u03b3\u03b9\u03ba\u03ae \u03bf\u03bc\u03b1\u03b4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7:</strong> \u039f\u03bc\u03b1\u03b4\u03bf\u03c0\u03bf\u03b9\u03b5\u03af \u03b1\u03c1\u03c7\u03b5\u03af\u03b1 \u03bc\u03b1\u03b6\u03af \u03c3\u03c4\u03bf \u03af\u03b4\u03b9\u03bf \u03bc\u03ad\u03c3\u03bf \u03c7\u03c1\u03b7\u03c3\u03b9\u03bc\u03bf\u03c0\u03bf\u03b9\u03ce\u03bd\u03c4\u03b1\u03c2 \u03bc\u03c0\u03b1\u03bb\u03b1\u03bd\u03c4\u03ad\u03c1, \u03ba\u03b1\u03bd\u03bf\u03bd\u03b9\u03ba\u03ad\u03c2 \u03b5\u03ba\u03c6\u03c1\u03ac\u03c3\u03b5\u03b9\u03c2 \u03ae \u03c3\u03b7\u03bc\u03b1\u03c3\u03b9\u03bf\u03bb\u03bf\u03b3\u03b9\u03ba\u03ae \u03c3\u03c5\u03c3\u03c4\u03b1\u03b4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7.",
        "commit_title": "\u0394\u03b5\u03c3\u03bc\u03b5\u03cd\u03c3\u03b5\u03b9\u03c2 \u0399\u03b4\u03b9\u03c9\u03c4\u03b9\u03ba\u03cc\u03c4\u03b7\u03c4\u03b1\u03c2 &amp; \u0391\u03c0\u03cc\u03b4\u03bf\u03c3\u03b7\u03c2",
        "commit_p1": "\u03c4\u03bf Burn to the Brim \u03b5\u03af\u03bd\u03b1\u03b9 \u03c3\u03c7\u03b5\u03b4\u03b9\u03b1\u03c3\u03bc\u03ad\u03bd\u03bf \u03bc\u03b5 \u03b3\u03bd\u03ce\u03bc\u03bf\u03bd\u03b1 \u03c4\u03b7\u03bd \u03b9\u03b4\u03b9\u03c9\u03c4\u03b9\u03ba\u03cc\u03c4\u03b7\u03c4\u03b1 \u03c4\u03bf\u03c5 \u03c7\u03c1\u03ae\u03c3\u03c4\u03b7, \u03c4\u03b7\u03bd \u03b1\u03c0\u03cc\u03b4\u03bf\u03c3\u03b7 \u03ba\u03b1\u03b9 \u03c4\u03b1 \u03ba\u03b1\u03b8\u03b1\u03c1\u03ac \u03c0\u03c1\u03cc\u03c4\u03c5\u03c0\u03b1 \u03bb\u03bf\u03b3\u03b9\u03c3\u03bc\u03b9\u03ba\u03bf\u03cd:",
        "commit_offline": "<strong>\u0391\u03c5\u03c3\u03c4\u03b7\u03c1\u03ac \u039f\u03c6\u03bb\u03ac\u03b9\u03bd \u0395\u03ba\u03c4\u03ad\u03bb\u03b5\u03c3\u03b7 (\u03a7\u03c9\u03c1\u03af\u03c2 Spyware):</strong> \u0397 \u03b5\u03c6\u03b1\u03c1\u03bc\u03bf\u03b3\u03ae \u03bb\u03b5\u03b9\u03c4\u03bf\u03c5\u03c1\u03b3\u03b5\u03af \u03b1\u03c0\u03bf\u03ba\u03bb\u03b5\u03b9\u03c3\u03c4\u03b9\u03ba\u03ac \u03c4\u03bf\u03c0\u03b9\u03ba\u03ac \u03c3\u03c4\u03bf \u03c3\u03cd\u03c3\u03c4\u03b7\u03bc\u03ac \u03c3\u03b1\u03c2. \u0394\u03b5\u03bd \u03ba\u03b1\u03c4\u03b1\u03b3\u03c1\u03ac\u03c6\u03b5\u03b9, \u03c0\u03b1\u03c1\u03b1\u03ba\u03bf\u03bb\u03bf\u03c5\u03b8\u03b5\u03af \u03ae \u03bc\u03b5\u03c4\u03b1\u03b4\u03af\u03b4\u03b5\u03b9 \u03c0\u03bf\u03c4\u03ad \u03c4\u03b9\u03c2 \u03b4\u03b9\u03b1\u03b4\u03c1\u03bf\u03bc\u03ad\u03c2 \u03c4\u03c9\u03bd \u03ba\u03b1\u03c4\u03b1\u03bb\u03cc\u03b3\u03c9\u03bd, \u03c4\u03b1 \u03bf\u03bd\u03cc\u03bc\u03b1\u03c4\u03b1 \u03b1\u03c1\u03c7\u03b5\u03af\u03c9\u03bd \u03ae \u03c4\u03b9\u03c2 \u03c0\u03c1\u03bf\u03c3\u03c9\u03c0\u03b9\u03ba\u03ad\u03c2 \u03c3\u03b1\u03c2 \u03c0\u03bb\u03b7\u03c1\u03bf\u03c6\u03bf\u03c1\u03af\u03b5\u03c2 \u03bc\u03ad\u03c3\u03c9 \u03b4\u03b9\u03ba\u03c4\u03cd\u03bf\u03c5.",
        "commit_adware": "<strong>\u03a7\u03c9\u03c1\u03af\u03c2 \u0394\u03b9\u03b1\u03c6\u03b7\u03bc\u03af\u03c3\u03b5\u03b9\u03c2 (Adware):</strong> \u0397 \u03b5\u03c6\u03b1\u03c1\u03bc\u03bf\u03b3\u03ae \u03b4\u03b5\u03bd \u03c0\u03b5\u03c1\u03b9\u03ad\u03c7\u03b5\u03b9 \u03b4\u03b9\u03b1\u03c6\u03b7\u03bc\u03af\u03c3\u03b5\u03b9\u03c2 \u03ae \u03b5\u03bd\u03bf\u03c7\u03bb\u03b7\u03c4\u03b9\u03ba\u03ac \u03c0\u03b1\u03c1\u03ac\u03b8\u03c5\u03c1\u03b1.",
        "commit_footprint": "<strong>\u0395\u03be\u03b1\u03b9\u03c1\u03b5\u03c4\u03b9\u03ba\u03ac \u0395\u03bb\u03b1\u03c6\u03c1\u03cd:</strong> \u039f \u03bc\u03b5\u03c4\u03b1\u03b3\u03bb\u03c9\u03c4\u03c4\u03b9\u03c3\u03bc\u03ad\u03bd\u03bf\u03c2 C++ \u03ba\u03ce\u03b4\u03b9\u03ba\u03b1\u03c2 \u03b5\u03be\u03b1\u03c3\u03c6\u03b1\u03bb\u03af\u03b6\u03b5\u03b9 \u03b5\u03bb\u03ac\u03c7\u03b9\u03c3\u03c4\u03b7 \u03c7\u03c1\u03ae\u03c3\u03b7 \u03bc\u03bd\u03ae\u03bc\u03b7\u03c2.",
        "commit_clutter": "<strong>\u039a\u03b1\u03b8\u03b1\u03c1\u03cc \u03a3\u03cd\u03c3\u03c4\u03b7\u03bc\u03b1:</strong> \u03a4\u03bf \u03c0\u03c1\u03cc\u03b3\u03c1\u03b1\u03bc\u03bc\u03b1 \u03b5\u03b3\u03ba\u03b1\u03c4\u03ac\u03c3\u03c4\u03b1\u03c3\u03b7\u03c2 \u03ba\u03b1\u03c4\u03b1\u03c7\u03c9\u03c1\u03b5\u03af \u03bc\u03cc\u03bd\u03bf \u03c4\u03b1 \u03b1\u03c0\u03b1\u03c1\u03b1\u03af\u03c4\u03b7\u03c4\u03b1 \u03b3\u03b9\u03b1 \u03c4\u03b7\u03bd \u03b5\u03bd\u03c3\u03c9\u03bc\u03ac\u03c4\u03c9\u03c3\u03b7 \u03c3\u03c4\u03b1 Windows.",
        "commit_uninst": "<strong>\u039a\u03b1\u03b8\u03b1\u03c1\u03ae \u0391\u03c0\u03b5\u03b3\u03ba\u03b1\u03c4\u03ac\u03c3\u03c4\u03b1\u03c3\u03b7:</strong> \u0397 \u03c4\u03c5\u03c0\u03b9\u03ba\u03ae \u03b1\u03c0\u03b5\u03b3\u03ba\u03b1\u03c4\u03ac\u03c3\u03c4\u03b1\u03c3\u03b7 \u03ba\u03b1\u03b8\u03b1\u03c1\u03af\u03b6\u03b5\u03b9 \u03c0\u03bb\u03ae\u03c1\u03c9\u03c2 \u03cc\u03bb\u03bf\u03c5\u03c2 \u03c4\u03bf\u03c5\u03c2 \u03c6\u03b1\u03ba\u03ad\u03bb\u03bf\u03c5\u03c2 \u03c4\u03bf\u03c5 \u03c0\u03c1\u03bf\u03b3\u03c1\u03ac\u03bc\u03bc\u03b1\u03c4\u03bf\u03c2.",
        "copyright": "&copy; \u039f\u03bc\u03ac\u03b4\u03b1 BTTB. \u039c\u03b5 \u03b5\u03c0\u03b9\u03c6\u03cd\u03bb\u03b1\u03be\u03b7 \u03ba\u03ac\u03b8\u03b5 \u03b4\u03b9\u03ba\u03b1\u03b9\u03ce\u03bc\u03b1\u03c4\u03bf\u03c2.",
        "motto": "\u0391\u03c1\u03c7\u03b5\u03b9\u03bf\u03b8\u03b5\u03c4\u03ae\u03c3\u03c4\u03b5 \u03ad\u03be\u03c5\u03c0\u03bd\u03b1"
    },
    "la": {
        "title": "Burn to the Brim: Conspectus",
        "h1": "Conspectus Applicationis",
        "subtitle": "Instrumentum ad spatium servandum et catalogos optime componendos.",
        "desc_title": "Descriptio Producti",
        "desc_p1": "<strong>Burn to the Brim</strong> est instrumentum peritum ad fasciculos colligendos et spatium condendi optimum faciendum. Solvit problema classicum bin-packing et subset-sum, sinit te fasciculos et catalogos perfecte in instrumenta condendi componere\u2014ut CD, DVD, Blu-ray, USB aut discos virtuales propriae capacitatis\u2014cum paene nullo spatio perdito.",
        "desc_p2": "Adhibens solverem backtracking recursivum maxime optimizatum, haec applicatio structuras catalogorum et magnitudines fasciculorum in millisecondis computat ad optimas formas componendas. Engine multithreaded non impediente, interface usoris fluit et interactive manet etiam cum magna data tractantur.",
        "features_title": "Res Praecipuae",
        "feat_span": "<strong>Compositio Multivoluminum:</strong> Magnas fasciculorum structuras quae capacitatem unius disci excedunt in catalogos volumines componit.",
        "feat_par3": "<strong>Cauchy Reed-Solomon PAR3 Paritas:</strong> Archiva paritatis PAR3 ad errores corrigendos generat, damnum spatii in computando computans.",
        "feat_verify": "<strong>Integris Inspectio et Recuperatio:</strong> Volumina inspectat contra archiva paritatis ut res corruptas inveniat et eas restituat.",
        "feat_menu": "<strong>Integratio in Contextu Explorer:</strong> Statim sinit addere fasciculos ex menu dextri click in Windows Explorer.",
        "feat_cli": "<strong>Machina GUI/CLI Hybrida:</strong> Facile mutatur inter interface leve nativum (Win32 / GTK 4) et lineam imperii ad scripta facienda.",
        "feat_unicode": "<strong>Unicode et Semitae Longae:</strong> Plene sustinet characteres internationales et semitas longas Windows UNC usque ad 32,767 characteres.",
        "feat_render": "<strong>Repraesentatio Arboris Non Impediens:</strong> Magnas series datorum paulatim ostendit temporibus otiosis principalis circuitus ne GUI rigescat.",
        "feat_precision": "<strong>MB Capacitas Exacta:</strong> Sustinet accurate capacitatis valores, sicut volumina 180 KB aut 0.18 MB, ad componendum perfecte.",
        "feat_json": "<strong>Catalogatio JSON Offline:</strong> Index <code>index.json</code> creatur ad res inspectandas sine conexione interretiali.",
        "feat_semantic": "<strong>Ordinatio Regulis et Semantica:</strong> Fasciculos in eodem volumine colligit utendo signis, regulis regularibus vel congregatione semantica.",
        "commit_title": "Fides Privatae et Celeritatis",
        "commit_p1": "Burn to the Brim factum est respiciens usoris secretum, systematis celeritatem, et munditiem programmatum:",
        "commit_offline": "<strong>Strictus Off-line Cursus (Sine Spyware):</strong> Haec applicatio localiter tantum currit. Numquam semitas, nomina fasciculorum aut res privatas ad rete mittit.",
        "commit_adware": "<strong>Sine Adware:</strong> Programma nullo modo praeconia aut fenestras invitantes continet.",
        "commit_footprint": "<strong>Minimus Rerum Usus:</strong> C++ code nativum statice compilatum minimam memoriam occupat.",
        "commit_clutter": "<strong>Nihil Supervacaneum:</strong> Programma tantum res necessarias ad integrationem Windows scribit, neque registrum inquinat.",
        "commit_uninst": "<strong>Munda Deinstalatio:</strong> Cum deinstallatur, omnia de programmate ex systemate funditus tolluntur.",
        "copyright": "&copy; BTTB Team. Omnia jura reservata.",
        "motto": "Intelligenter archiva facere"
    },
    "pt": {
        "title": "Burn to the Brim: Vis\u00e3o Geral",
        "h1": "Vis\u00e3o Geral do Aplicativo",
        "subtitle": "Um otimizador de espa\u00e7o de armazenamento de alto desempenho e utilit\u00e1rio de empacotamento de diret\u00f3rios.",
        "desc_title": "Descri\u00e7\u00e3o do Produto",
        "desc_p1": "<strong>Burn to the Brim</strong> \u00e9 um utilit\u00e1rio profissional de empacotamento de arquivos e otimiza\u00e7\u00e3o de armazenamento. Resolve o cl\u00e1ssico problema do empacotamento (bin-packing) e da soma de subconjuntos, permitindo que voc\u00ea ajuste arquivos e pastas perfeitamente em m\u00eddias de armazenamento de destino\u2014incluindo CDs, DVDs, Blu-rays grav\u00e1veis, unidades flash USB ou discos virtuais de capacidade personalizada\u2014com praticamente zero desperd\u00edcio de espa\u00e7o.",
        "desc_p2": "Empregando um mecanismo de resolu\u00e7\u00e3o por backtracking recursivo altamente otimizado, o aplicativo analisa estruturas de diret\u00f3rios e tamanhos de arquivos para calcular configura\u00e7\u00f5es de layout quase perfeitas em milissegundos. Com um motor multithread n\u00e3o bloqueante, a interface do usu\u00e1rio permanece completamente fluida e interativa, mesmo ao calcular solu\u00e7\u00f5es para conjuntos de dados muito grandes.",
        "features_title": "Principais Recursos",
        "feat_span": "<strong>Divis\u00e3o em M\u00faltiplos Volumes:</strong> Divide automaticamente grandes hierarquias de arquivos que excedem a capacidade de um \u00fanico disco em pastas de volumes com tamanhos otimizados.",
        "feat_par3": "<strong>Paridade PAR3 Cauchy Reed-Solomon:</strong> Cria automaticamente arquivos de paridade PAR3 para corre\u00e7\u00e3o de erros, considerando o custo adicional diretamente nas verifica\u00e7\u00f5es de layout.",
        "feat_verify": "<strong>Verifica\u00e7\u00e3o de Integridade e Recupera\u00e7\u00e3o:</strong> Verifica volumes contra arquivos de paridade para identificar itens corrompidos e realizar restaura\u00e7\u00e3o baseada em c\u00f3pia.",
        "feat_menu": "<strong>Integra\u00e7\u00e3o com o Menu de Contexto do Explorador:</strong> Adiciona arquivos e pastas instantaneamente a sess\u00f5es ativas usando o menu de clique do bot\u00e3o direito do Windows Explorer.",
        "feat_cli": "<strong>Motor H\u00edbrido GUI/CLI:</strong> Alterne de forma transparente entre uma interface de usu\u00e1rio nativa leve (Win32 / GTK 4) e um modo de linha de comando completo para scripts.",
        "feat_unicode": "<strong>Suporte Unicode e Caminhos Longos:</strong> Totalmente compat\u00edvel com codifica\u00e7\u00f5es de caracteres internacionais e caminhos longos UNC do Windows de at\u00e9 32.767 caracteres.",
        "feat_render": "<strong>Renderiza\u00e7\u00e3o de \u00c1rvore N\u00e3o Bloqueante:</strong> Exibe grandes conjuntos de dados de forma incremental usando os ciclos ociosos do loop principal para evitar congelamento da GUI.",
        "feat_precision": "<strong>Precis\u00e3o de Capacidade em fra\u00e7\u00f5es de MB:</strong> Suporta entradas e salvamentos precisos de capacidade, como volumes personalizados de 180 KB ou 0.18 MB, para empacotamento exato.",
        "feat_json": "<strong>Indexa\u00e7\u00e3o JSON Offline:</strong> Gera um cat\u00e1logo de diret\u00f3rios padronizado <code>index.json</code> ao gravar os arquivos, listando atributos para navega\u00e7\u00e3o offline.",
        "feat_semantic": "<strong>Agrupamento Sem\u00e2ntico e por Regras:</strong> Agrupa arquivos na mesma m\u00eddia usando curingas padr\u00e3o, express\u00f5es regulares ou agrupamento sem\u00e2ntico contextual.",
        "commit_title": "Compromissos de Privacidade e Desempenho",
        "commit_p1": "O Burn to the Brim \u00e9 constru\u00eddo com a privacidade do usu\u00e1rio, desempenho do sistema e padr\u00f5es de software limpo em mente:",
        "commit_offline": "<strong>Execu\u00e7\u00e3o Offline Estrita (Sem Spyware):</strong> O aplicativo funciona inteiramente de forma local. Ele nunca registra, rastreia ou transmite suas rotas, nomes de arquivos ou dados pessoais pela rede.",
        "commit_adware": "<strong>Sem Adware:</strong> O aplicativo n\u00e3o cont\u00e9m an\u00fancios, banners comerciais ou pop-ups patrocinados.",
        "commit_footprint": "<strong>Uso Ultra-Leve de Recursos:</strong> C\u00f3digo nativo em C++ compilado estaticamente garante consumo m\u00ednimo de mem\u00f3ria e elimina qualquer sobrecarga de frameworks.",
        "commit_clutter": "<strong>Sem Lixo no Sistema:</strong> O instalador registra apenas o necess\u00e1rio para a integra\u00e7\u00e3o do Windows, sem poluir o registro do sistema.",
        "commit_uninst": "<strong>Desinstala\u00e7\u00e3o Limpa:</strong> O processo padr\u00e3o de desinstala\u00e7\u00e3o remove completamente todas as pastas e atalhos do aplicativo do seu sistema.",
        "copyright": "&copy; Equipe BTTB. Todos os direitos reservados.",
        "motto": "Arquive com intelig\u00eancia"
    },    "hi": {
        "title": "Burn to the Brim: अवलोकन",
        "h1": "एप्लिकेशन अवलोकन",
        "subtitle": "एक उच्च-प्रदर्शन भंडारण स्थान अनुकूलक और निर्देशिका पैकिंग उपयोगिता।",
        "desc_title": "उत्पाद विवरण",
        "desc_p1": "<strong>Burn to the Brim</strong> एक पेशेवर फ़ाइल पैकेजिंग और स्टोरेज अनुकूलन उपयोगिता है। यह क्लासिक बिन-पैकिंग और सबसेट-सम अनुकूलन चुनौती को हल करता है, जिससे आप फ़ाइलों और फ़ोल्डरों को लक्षित भंडारण माध्यमों—जिसमें रिकॉर्ड करने योग्य CD, DVD, Blu-ray, USB फ्लैश ड्राइव, या कस्टम-क्षमता वाले वर्चुअल ड्राइव शामिल हैं—पर व्यावहारिक रूप से शून्य बर्बाद स्थान के साथ पूरी तरह से फिट कर सकते हैं।",
        "desc_p2": "एक अत्यधिक अनुकूलित पुनरावर्ती बैकट्रैकिंग सॉल्वर का उपयोग करते हुए, एप्लिकेशन मिलीसेकंड में लगभग-सही लेआउट कॉन्फ़िगरेशन की गणना करने के लिए निर्देशिका संरचनाओं और फ़ाइल आकारों का विश्लेषण करता है। एक गैर-अवरोधक मल्टीथ्रेडेड इंजन के साथ, यूजर इंटरफेस बहुत बड़े डेटासेट के लिए समाधान की गणना करते समय भी पूरी तरह से सहज और इंटरैक्टिव बना रहता है।",
        "features_title": "मुख्य विशेषताएं",
        "feat_span": "<strong>मल्टी-वॉल्यूम स्पैनिंग:</strong> एकल-ड्राइव क्षमताओं से अधिक बड़ी फ़ाइल पदानुक्रमों को स्वचालित रूप से इष्टतम रूप से आकार वाले वॉल्यूम फ़ोल्डरों में विभाजित करता है।",
        "feat_par3": "<strong>कोशी रीड-सोलोमन PAR3 पैरिटी:</strong> त्रुटि सुधार के लिए स्वचालित रूप से PAR3 पैरिटी अभिलेखागार बनाता है, लेआउट जांच में सीधे ओवरहेड को शामिल करता है।",
        "feat_verify": "<strong>सत्यता जांच और बहाली:</strong> क्षतिग्रस्त वस्तुओं की पहचान करने और गैर-विनाशकारी पुनर्स्थापना करने के लिए पैरिटी फ़ाइलों के विरुद्ध वॉल्यूम को स्कैन और सत्यापित करता है।",
        "feat_menu": "<strong>एक्स्प्लोरर संदर्भ मेनू एकीकरण:</strong> विंडोज एक्सप्लोरर राइट-क्लिक मेनू के माध्यम से सक्रिय सत्रों में सीधे फ़ाइलों और फ़ोल्डरों को तुरंत जोड़ता है।",
        "feat_cli": "<strong>हाइब्रिड GUI/CLI इंजन:</strong> स्क्रिप्टिंग के लिए एक हल्के मूल यूजर इंटरफेस (Win32 / GTK 4) और एक पूर्ण कमांड-लाइन मोड के बीच आसानी से स्विच करें।",
        "feat_unicode": "<strong>यूनिकोड और लंबे पथ समर्थन:</strong> अंतर्राष्ट्रीय वर्ण एन्कोडिंग और 32,767 वर्णों तक के विंडोज UNC लंबे पथों के साथ पूरी तरह से संगत।",
        "feat_render": "<strong>गैर-अवरोधक ट्री रेंडरिंग:</strong> GUI को फ्रीज होने से बचाने के लिए मुख्य-लूप के खाली समय का उपयोग करके बड़े समाधान डेटासेट को क्रमिक रूप से दिखाता है।",
        "feat_precision": "<strong>दशमलव MB क्षमता सटीकता:</strong> सटीक पैकिंग के लिए सटीक क्षमता इनपुट और बचत का समर्थन करता है, जैसे कि कस्टम 180 KB या 0.18 MB वॉल्यूम।",
        "feat_json": "<strong>ऑफ़लाइन JSON अनुक्रमणिका:</strong> आउटपुट के दौरान एक मानकीकृत <code>index.json</code> निर्देशिका कैटलॉग उत्पन्न करता है, जो ऑफ़लाइन ब्राउज़िंग के लिए फ़ाइल विशेषताओं को सूचीबद्ध करता है।",
        "feat_semantic": "<strong>नियम-आधारित और सिमेंटिक समूहीकरण:</strong> मानक वाइल्डकार्ड, नियमित अभिव्यक्ति, या संदर्भ-जागरूक सिमेंटिक क्लस्टरिंग का उपयोग करके फाइलों को एक ही माध्यम पर एक साथ समूहित करता है।",
        "commit_title": "गोपनीयता और प्रदर्शन प्रतिबद्धताएं",
        "commit_p1": "Burn to the Brim उपयोगकर्ता की गोपनीयता, सिस्टम प्रदर्शन और स्वच्छ सॉफ़्टवेयर मानकों को ध्यान में रखकर बनाया गया है:",
        "commit_offline": "<strong>सख्त ऑफ़लाइन निष्पादन (कोई स्पाइवेयर नहीं):</strong> एप्लिकेशन पूरी तरह से आपके सिस्टम पर स्थानीय रूप से काम करता है। यह नेटवर्क पर आपके निर्देशिका पथ, फ़ाइल नाम या व्यक्तिगत जानकारी को कभी भी रिकॉर्ड, ट्रैक या प्रसारित नहीं करता है।",
        "commit_adware": "<strong>कोई एडवेयर नहीं:</strong> एप्लिकेशन में कोई विज्ञापन, व्यावसायिक बैनर या प्रायोजित पॉपअप शामिल नहीं हैं।",
        "commit_footprint": "<strong>अति-हल्का संसाधन उपयोग:</strong> स्थिर रूप से संकलित मूल C++ कोड न्यूनतम मेमोरी उपयोग और शून्य फ्रेमवर्क ओवरहेड सुनिश्चित करता है।",
        "commit_clutter": "<strong>शून्य सिस्टम अव्यवस्था:</strong> इंस्टॉलर केवल वही पंजीकृत करता है जो विंडोज एकीकरण के लिए आवश्यक है, और एप्लिकेशन आपकी रजिस्ट्री को कचरा फ़ाइलों से गंदा नहीं करता है।",
        "commit_uninst": "<strong>स्वच्छ अनइंस्टॉलेशन:</strong> मानक अनइंस्टॉल प्रक्रिया आपके सिस्टम से सभी प्रोग्राम निर्देशिकाओं और शॉर्टकट को पूरी तरह से साफ कर देती है।",
        "copyright": "&copy; BTTB टीम। सभी अधिकार सुरक्षित।",
        "motto": "बुद्धिमानी से संग्रह करें"
    },
    "vul": {
        "title": "Burn to the Brim: Spohk",
        "h1": "Kuv-Spohk",
        "subtitle": "Krus t'bakh shem t'stor-spahk eh ves-pak tor.",
        "desc_title": "Ves-Ves-T'lik",
        "desc_p1": "<strong>Burn to the Brim</strong> gu kuv-spohk t'shik t'stor eh bakh-tor. Ku solve bin-packing eh subset-sum t'shik, spohk files eh folders t'dest-stor\u2014CD, DVD, Blu-ray, USB eh custom-capacity virtual drives\u2014va zero space was-tor.",
        "desc_p2": "Spohk t'backtracking solver, ves kuv-spohk structures eh files t'kahr-tor in milliseconds. T'non-blocking engine, user-interface spohk fluid eh interactive.",
        "features_title": "Spohk-T'lik",
        "feat_span": "<strong>Multi-Volume Spanning:</strong> Spohk automatic split files t'exceed single-capacity t'vol-folders.",
        "feat_par3": "<strong>Cauchy Reed-Solomon PAR3 Parity:</strong> Spohk automatic create PAR3 t'error correction.",
        "feat_verify": "<strong>Integrity Verification &amp; Recovery:</strong> Scan volumes t'detect corrupt items eh restore.",
        "feat_menu": "<strong>Explorer Context Menu:</strong> Spohk direct append files va Windows Explorer context menu.",
        "feat_cli": "<strong>Hybrid GUI/CLI:</strong> Switch GUI (Win32 / GTK 4) eh CLI mode.",
        "feat_unicode": "<strong>Unicode &amp; Long Paths:</strong> Compatible va international char-set eh Windows long UNC paths.",
        "feat_render": "<strong>Non-Blocking Tree Rendering:</strong> Render large datasets va idle cycles t'prevent GUI freeze.",
        "feat_precision": "<strong>Fractional MB Precision:</strong> Support custom 180 KB eh 0.18 MB volumes.",
        "feat_json": "<strong>Offline JSON Indexing:</strong> Generate <code>index.json</code> t'offline browsing.",
        "feat_semantic": "<strong>Rule-Based &amp; Semantic Grouping:</strong> Group files va wildcards eh MiniLM semantic clustering.",
        "commit_title": "Privacy eh Performance",
        "commit_p1": "Burn to the Brim gu privacy eh performance t'shik:",
        "commit_offline": "<strong>Strict Offline (No Spyware):</strong> Operating entirely locally. Never logs, tracks or transmits personal info.",
        "commit_adware": "<strong>No Adware:</strong> No ads or sponsored popups.",
        "commit_footprint": "<strong>Ultra-Lightweight:</strong> Native C++ code ensures minimal memory usage.",
        "commit_clutter": "<strong>Zero Clutter:</strong> Installer registers only necessary settings.",
        "commit_uninst": "<strong>Clean Uninstallation:</strong> Remove all settings eh program directories.",
        "copyright": "&copy; BTTB Team. Duf-t'lik.",
        "motto": "Kuv-stor shanik"
    },
    "elv": {
        "title": "Burn to the Brim: Cennas",
        "h1": "Cennas o Firiar",
        "subtitle": "M\u00e1re tancal\u00eb o cormar ar taure carm\u00eb.",
        "desc_title": "Sinya Canna",
        "desc_p1": "<strong>Burn to the Brim</strong> n\u00e1 arda m\u00e1ra firiar ar cormar tancal\u00eb. Car\u00eb solving bin-packing ar subset-sum, firiar firiar ar cormar target cormar\u2014CDs, DVDs, Blu-rays, USBs ar custom-capacity virtual drives\u2014va zero wasted space.",
        "desc_p2": "Employing highly optimized recursive backtracking solver, cennas o firiar in milliseconds. With non-blocking multithreaded engine, user-interface remains fluid ar interactive.",
        "features_title": "M\u00e1ra Carm\u00eb",
        "feat_span": "<strong>Multi-Volume Spanning:</strong> Splits file hierarchies automatically into optimally sized volume folders.",
        "feat_par3": "<strong>Cauchy Reed-Solomon PAR3 Parity:</strong> Creates PAR3 parity archives automatically for error correction.",
        "feat_verify": "<strong>Integrity Verification &amp; Recovery:</strong> Scan volumes against parity files to identify corrupted items.",
        "feat_menu": "<strong>Explorer Context Menu:</strong> Append files directly via Explorer context menu.",
        "feat_cli": "<strong>Hybrid GUI/CLI Engine:</strong> Switch between lightweight GUI (Win32 / GTK 4) and CLI.",
        "feat_unicode": "<strong>Unicode &amp; Long Paths:</strong> Compatible with international character sets and UNC long paths.",
        "feat_render": "<strong>Non-Blocking Tree Rendering:</strong> Displays large solution datasets incrementally to prevent GUI freezing.",
        "feat_precision": "<strong>Fractional MB Capacity Precision:</strong> Supports precise capacity inputs like 180 KB or 0.18 MB.",
        "feat_json": "<strong>Offline JSON Indexing:</strong> Generates a standardized <code>index.json</code> directory catalog.",
        "feat_semantic": "<strong>Rule-Based &amp; Semantic Grouping:</strong> Group files using wildcards or context-aware semantic clustering.",
        "commit_title": "Vanda o Secrecy ar Celerity",
        "commit_p1": "Burn to the Brim is built with user privacy and system performance in mind:",
        "commit_offline": "<strong>Strict Offline (No Spyware):</strong> Operating entirely locally. Never transmits directory paths or file names.",
        "commit_adware": "<strong>No Adware:</strong> Contains no advertisements or sponsored popups.",
        "commit_footprint": "<strong>Ultra-Lightweight:</strong> Native C++ code ensures minimal memory usage.",
        "commit_clutter": "<strong>Zero Clutter:</strong> Installer registers only necessary settings.",
        "commit_uninst": "<strong>Clean Uninstallation:</strong> Removes all files and directories completely from your system.",
        "copyright": "&copy; BTTB Team. Valar alya.",
        "motto": "Hosta vanya"
    }
}

# Add missing translations for German (de), French (fr), Spanish (es), Dutch (nl) matching the template keys
# (already embedded in the values above)

HTML_TEMPLATE = """<!DOCTYPE html>
<html lang="{lang_code}">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{title}</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&family=Outfit:wght@400;500;600;700;800&display=swap');

        :root {{
            --bg-main: #0B0F19;
            --bg-card: #151D30;
            --bg-sidebar: #0D1424;
            --accent: #6366F1;
            --accent-hover: #4F46E5;
            --text-primary: #F3F4F6;
            --text-secondary: #9CA3AF;
            --border-color: #24324F;
            --success: #10B981;
        }}

        * {{
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }}

        body {{
            background-color: var(--bg-main);
            color: var(--text-primary);
            font-family: 'Inter', sans-serif;
            display: flex;
            min-height: 100vh;
            overflow-x: hidden;
        }}

        /* Sidebar navigation */
        .sidebar {{
            width: 260px;
            background-color: var(--bg-sidebar);
            border-right: 1px solid var(--border-color);
            padding: 30px 20px;
            display: flex;
            flex-direction: column;
            position: fixed;
            height: 100vh;
            overflow-y: auto;
            z-index: 100;
        }}

        .logo-container {{
            display: flex;
            flex-direction: column;
            align-items: center;
            margin-bottom: 40px;
            text-align: center;
        }}

        .logo-img {{
            filter: drop-shadow(0 4px 12px rgba(99, 102, 241, 0.4));
            transition: transform 0.3s ease;
        }}

        .logo-img:hover {{
            transform: scale(1.08);
        }}

        .logo-title {{
            font-family: 'Outfit', sans-serif;
            font-size: 1.25rem;
            font-weight: 700;
            margin-top: 15px;
            background: linear-gradient(135deg, #A5B4FC, var(--accent));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }}

        .nav-links {{
            list-style: none;
            display: flex;
            flex-direction: column;
            gap: 8px;
        }}

        .nav-item a {{
            display: flex;
            align-items: center;
            padding: 12px 16px;
            border-radius: 8px;
            color: var(--text-secondary);
            text-decoration: none;
            font-weight: 500;
            font-size: 0.95rem;
            transition: all 0.2s ease;
            border: 1px solid transparent;
        }}

        .nav-item.active a,
        .nav-item a:hover {{
            color: var(--text-primary);
            background-color: rgba(99, 102, 241, 0.15);
            border-color: rgba(99, 102, 241, 0.25);
        }}

        .nav-item.active a {{
            box-shadow: 0 4px 12px rgba(99, 102, 241, 0.1);
        }}

        .sf-logo {{
            margin-top: auto;
            padding-top: 30px;
            display: flex;
            justify-content: center;
        }}

        .sf-logo img {{
            opacity: 0.7;
            transition: opacity 0.2s ease;
        }}

        .sf-logo img:hover {{
            opacity: 1;
        }}

        /* Main Content */
        .main-content {{
            margin-left: 260px;
            flex: 1;
            padding: 40px 50px;
            max-width: 1200px;
        }}

        .section-header {{
            margin-bottom: 40px;
        }}

        .section-header h1 {{
            font-family: 'Outfit', sans-serif;
            font-size: 2.5rem;
            font-weight: 800;
            margin-bottom: 10px;
            background: linear-gradient(135deg, #FFF, #A5B4FC);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }}

        .section-header p {{
            color: var(--text-secondary);
            font-size: 1.1rem;
        }}

        /* Content Blocks */
        .content-card {{
            background-color: var(--bg-card);
            border: 1px solid var(--border-color);
            border-radius: 16px;
            padding: 35px;
            margin-bottom: 30px;
            box-shadow: 0 10px 25px rgba(0,0,0,0.15);
        }}

        .content-card h2 {{
            font-family: 'Outfit', sans-serif;
            font-size: 1.5rem;
            font-weight: 700;
            margin-bottom: 20px;
            color: var(--accent);
            display: flex;
            align-items: center;
            gap: 10px;
        }}

        .content-card p {{
            color: var(--text-secondary);
            font-size: 1.05rem;
            line-height: 1.6;
            margin-bottom: 15px;
        }}

        .feature-bullets {{
            list-style: none;
            display: flex;
            flex-direction: column;
            gap: 12px;
            margin-top: 20px;
        }}

        .feature-bullet-item {{
            display: flex;
            align-items: flex-start;
            gap: 12px;
            color: var(--text-secondary);
            font-size: 1rem;
            line-height: 1.5;
        }}

        .bullet-icon {{
            color: var(--success);
            font-weight: bold;
            flex-shrink: 0;
        }}

        .bullet-icon.red {{
            color: #EF4444;
        }}

        /* Footer */
        footer {{
            border-top: 1px solid var(--border-color);
            padding: 25px 0;
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 0.8rem;
            color: var(--text-secondary);
        }}

        /* Responsive adjustments */
        @media (max-width: 900px) {{
            body {{
                flex-direction: column;
            }}
            .sidebar {{
                width: 100%;
                height: auto;
                position: relative;
                border-right: none;
                border-bottom: 1px solid var(--border-color);
                padding: 20px;
            }}
            .logo-container {{
                margin-bottom: 20px;
                flex-direction: row;
                gap: 15px;
                align-items: center;
            }}
            .logo-title {{
                margin-top: 0;
            }}
            .nav-links {{
                flex-direction: row;
                flex-wrap: wrap;
                gap: 6px;
            }}
            .nav-item a {{
                padding: 8px 12px;
                font-size: 0.85rem;
            }}
            .sf-logo {{
                display: none;
            }}
            .main-content {{
                margin-left: 0;
                padding: 30px 20px;
            }}
        }}
    </style>
</head>
<body>

    <!-- Sidebar navigation -->
    <div class="sidebar">
        <div class="logo-container">
            <img class="logo-img" src="bttb.png" height="64" width="64" alt="BTTB Logo">
            <div class="logo-title">Burn to the Brim</div>
        </div>
        <ul class="nav-links">
            <li class="nav-item"><a href="index.html">{nav_home}</a></li>
            <li class="nav-item active"><a href="overview.html">{nav_overview}</a></li>
            <li class="nav-item"><a href="news.html">{nav_news}</a></li>
            <li class="nav-item"><a href="history.html">{nav_history}</a></li>
            <li class="nav-item"><a href="http://sourceforge.net/project/showfiles.php?group_id=49999" target="_blank">{nav_download}</a></li>
            <li class="nav-item"><a href="https://sourceforge.net/forum/forum.php?forum_id=164745" target="_blank">{nav_forum}</a></li>
            <li class="nav-item"><a href="contact.html">{nav_contact}</a></li>
            <li class="nav-item"><a href="elwin.html">{nav_about}</a></li>
            <li class="nav-item"><a href="Help">{nav_help}</a></li>
        </ul>
        <div class="sf-logo">
            <a href="https://sourceforge.net/projects/bttb/" target="_blank">
                <img src="http://sourceforge.net/sflogo.php?group_id=49999&type=1" width="88" height="31" border="0" alt="SourceForge.net Logo">
            </a>
        </div>
    </div>

    <!-- Main Content -->
    <div class="main-content">
        
        <div class="section-header">
            <h1>{h1}</h1>
            <p>{subtitle}</p>
            <div style="margin-top: 15px; font-size: 0.9rem; display: flex; flex-wrap: wrap; gap: 8px 10px; align-items: center; line-height: 1.5;">
                <span style="color: var(--text-secondary);">Languages:</span>
                {lang_bar_links}
            </div>
        </div>

        <!-- Store Description -->
        <div class="content-card">
            <h2>{desc_title}</h2>
            <p>{desc_p1}</p>
            <p>{desc_p2}</p>
        </div>

        <!-- Key Features -->
        <div class="content-card">
            <h2>{features_title}</h2>
            <ul class="feature-bullets">
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_span}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_par3}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_verify}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_menu}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_cli}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_unicode}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_render}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_precision}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_json}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon">&rarr;</span>
                    <div>{feat_semantic}</div>
                </li>
            </ul>
        </div>

        <!-- Privacy & Performance Commitments -->
        <div class="content-card">
            <h2>{commit_title}</h2>
            <p>{commit_p1}</p>
            <ul class="feature-bullets">
                <li class="feature-bullet-item">
                    <span class="bullet-icon red">&#10007;</span>
                    <div>{commit_offline}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon red">&#10007;</span>
                    <div>{commit_adware}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon red">&#10007;</span>
                    <div>{commit_footprint}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon red">&#10007;</span>
                    <div>{commit_clutter}</div>
                </li>
                <li class="feature-bullet-item">
                    <span class="bullet-icon red">&#10007;</span>
                    <div>{commit_uninst}</div>
                </li>
            </ul>
        </div>

        <!-- Footer -->
        <footer>
            <span>{copyright}</span>
            <span>{motto}</span>
        </footer>

    </div>

</body>
</html>
"""

def generate_language_bar(current_lang):
    links = []
    # Order languages by key for consistency
    ordered_keys = ["en", "es", "fr", "nl", "de", "zh", "ja", "it", "el", "la", "pt", "hi", "vul", "elv"]
    for key in ordered_keys:
        lang_info = LANGUAGES[key]
        if key == current_lang:
            links.append(f'<a id="lang_{key}" href="{lang_info["filename"]}" style="color: var(--accent); text-decoration: none; font-weight: 600;">{lang_info["name"]}</a>')
        else:
            links.append(f'<a id="lang_{key}" href="{lang_info["filename"]}" style="color: var(--text-secondary); text-decoration: none; transition: color 0.2s;" onmouseover="this.style.color=\'var(--text-primary)\'" onmouseout="this.style.color=\'var(--text-secondary)\'">{lang_info["name"]}</a>')
    return " |\n                ".join(links)

def main():
    if not os.path.exists(HTDOCS_DIR):
        print(f"Error: Target directory {HTDOCS_DIR} does not exist.")
        sys.exit(1)

    for lang_code, lang_content in CONTENT.items():
        navbar_trans = NAVBAR[lang_code]
        lang_bar = generate_language_bar(lang_code)
        
        # Merge dictionary
        merged = {}
        merged.update(lang_content)
        merged.update({
            "lang_code": lang_code,
            "lang_bar_links": lang_bar,
            "nav_home": navbar_trans["home"],
            "nav_overview": navbar_trans["overview"],
            "nav_news": navbar_trans["news"],
            "nav_history": navbar_trans["history"],
            "nav_download": navbar_trans["download"],
            "nav_forum": navbar_trans["forum"],
            "nav_contact": navbar_trans["contact"],
            "nav_about": navbar_trans["about"],
            "nav_help": navbar_trans["help"]
        })
        
        output_html = HTML_TEMPLATE.format(**merged)
        output_filename = LANGUAGES[lang_code]["filename"]
        output_path = os.path.join(HTDOCS_DIR, output_filename)
        
        with open(output_path, "w", encoding="utf-8") as f:
            f.write(output_html)
            
        print(f"Generated: {output_filename}")

if __name__ == "__main__":
    main()
