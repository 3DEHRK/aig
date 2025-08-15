TESTME — Manuelle Tests für Gameplay-Features

Zweck
Dieses Dokument listet manuelle Tests, die während der Entwicklung ausgeführt werden sollten. Für jede Funktion steht eine kurze Anleitung, wie man sie prüft und welches Ergebnis erwartet wird.

Hinweis: Vor jedem Test muss ein erfolgreicher Build vorhanden sein ("cmake --build ." ohne Fehler) und das Verzeichnis build/assets/ muss Assets enthalten.

1) Robuste Inventory-UI (IMPLEMENTIERT)
- Voraussetzung: Spiel startet ohne Fehler.
- Schritte:
  1. Starte das Spiel (./bin/sfml-game-framework).
  2. Öffne das Inventory-UI mit Taste I.
  3. Prüfe: UI zeigt ein Raster mit Slots, Itemnamen (oder Icons) und Stapelgrößen.
  4. Teste Drag & Drop: Linksklick auf ein Item, klicke auf eine andere Zelle, das Item sollte bewegt/vertauscht werden.
- Erwartetes Ergebnis: Items erscheinen im UI, Drag & Drop funktioniert, Stapelgrößen werden angezeigt.

2) Anpflanzen von Crops mit Samen (IMPLEMENTIERT)
- Voraussetzung: Crops-Assets vorhanden (assets/textures/entities/crop1.png ...). Player startet mit ein paar "seed_wheat" in Inventory.
- Schritte:
  1. Starte das Spiel und nähere dich einem Bereich mit Soil-Tiles (sichtbar als dunklere Kacheln).
  2. Drücke E, wenn du keine anderen Interaktionen hast; das System sucht in den umliegenden Kacheln nach Soil und pflanzt ein Crop (verbraucht 1 Seed).
  3. Warte ein paar Sekunden und beobachte, wie die Crop-Stages wechseln.
  4. Interagiere (E) an einer reifen Crop und prüfe, ob die Ernte ins Inventory geht.
- Erwartetes Ergebnis: Seed wird verbraucht, Crop erscheint auf Soil, Stages wechseln und Ernte landet im Inventar.

3) Eisenbahnsystem (Teil-IMPLEMENTIERT)
- Voraussetzung: Rail-Textur optional (assets/textures/tiles/rail.png). Einige Rail-Entitäten sind in PlayState gespawnt.
- Schritte:
  1. Starte das Spiel und schaue bei (200,200) nach Rail-Sprites/Platzhaltern.
  2. Prüfe: Rail-Entities sind sichtbar als dünne Balken oder Rail-Sprites.
- Erwartetes Ergebnis: Rail-Entities rendern; volle Bausystem-Logik noch ausstehend.

4) Feindliche NPCs, die attackieren (Teil-IMPLEMENTIERT)
- Voraussetzung: HostileNPC ist in PlayState gespawnt.
- Schritte:
  1. Starte das Spiel; ein feindlicher NPC sollte um (400,300) gespawnt sein.
  2. Nähere dich dem NPC; beobachte, ob er dir folgt und in Reichweite angreift.
- Erwartetes Ergebnis: HostileNPC verfolgt Spieler und entfernt zyklisch ein Item aus dem Inventory als einfacher Schadenstest.

5) Abwehrwaffe gegen NPCs (PENDING)
- Noch nicht implementiert: projectile/weapon system.

Allgemeine Hinweise
- Fehlerdiagnose ist einfacher, wenn die Konsole-Logs (stderr/stdout) offen sind — prüfe dort auf Pfad-/Asset-Fehler.
- Wenn ein Test fehlschlägt, notiere die Konsolenausgabe und die Schritte; ich kann dann den Fehler gezielt beheben.

Wenn du mir eine erfolgreiche CMake-Build-Ausgabe schickst, markiere ich die erledigten Tests hier im Dokument und fahre mit dem nächsten Roadmap-Punkt fort.
