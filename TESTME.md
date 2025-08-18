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

STATUS: OK (funktional; Positionierung relativ zum Spieler)

2) Anpflanzen von Crops mit Samen (IMPLEMENTIERT)
- Voraussetzung: Crops-Assets vorhanden (assets/textures/entities/crop1.png ...). Player startet mit ein paar "seed_wheat" in Inventory.
- Schritte:
  1. Starte das Spiel und nähere dich einem Bereich mit Soil-Tiles (sichtbar als dunklere Kacheln).
  2. Drücke E, wenn du keine anderen Interaktionen hast; das System sucht in den umliegenden Kacheln nach Soil und pflanzt ein Crop (verbraucht 1 Seed).
  3. Warte ein paar Sekunden und beobachte, wie die Crop-Stages wechseln.
  4. Interagiere (E) an einer reifen Crop und prüfe, ob die Ernte ins Inventory geht.
- Erwartetes Ergebnis: Seed wird verbraucht, Crop erscheint auf Soil, Stages wechseln und Ernte landet im Inventar.

STATUS: OK - Planting and crop rendering confirmed (console shows planting logs and crop creation)

3) Eisenbahnsystem (IMPLEMENTIERT - grundlegendes Bauwerkzeug)
- Voraussetzung: Rail-Textur optional (assets/textures/tiles/rail.png). Rail-Tool kann mit Taste 'B' umgeschaltet werden.
- Schritte:
  1. Starte das Spiel und drücke 'B' um den Rail-Bau-Modus zu aktivieren.
  2. Bewege die Maus über Kacheln, eine Vorschau zeigt die aktuelle Kachel an.
  3. Linksklick platziert eine Schiene auf der angezeigten Kachel; ein weiterer Klick entfernt sie.
  4. Beobachte: Die TileMap wird aktualisiert und Rail-Entities werden im Spiel sichtbar.
- Erwartetes Ergebnis: Du kannst Schienen platzieren und entfernen. Persistenz (Save/Load) der Schienen ist noch nicht implementiert.

STATUS: OK - texture scaling slightly small but functional

4) Feindliche NPCs, die attackieren (Teil-IMPLEMENTIERT)
- Voraussetzung: HostileNPC ist in PlayState gespawnt.
- Schritte:
  1. Starte das Spiel; ein feindlicher NPC sollte um (400,300) gespawnt sein.
  2. Nähere dich dem NPC; beobachte, ob er dir folgt und in Reichweite angreift.
- Erwartetes Ergebnis: HostileNPC verfolgt Spieler und entfernt zyklisch ein Item aus dem Inventory als einfacher Schadenstest.

5) Abwehrwaffe gegen NPCs (PENDING)
- Noch nicht implementiert: projectile/weapon system.

6) Exploration & Hidden Discoveries (NEW)
- Voraussetzung: Spiel startet; HiddenLocation test marker present at tile (10,10).
- Schritte:
  1. Starte das Spiel and move the player to tile (10,10) area.
  2. Observe a purple marker indicating an undiscovered hidden location. Interact with it (E or click).
  3. Expect: The marker disappears, a "Mysterious Dongle" is added to inventory, and a console log acknowledges discovery.
- Erwartetes Ergebnis: Player receives a dongle item and the tile is marked discovered.

STATUS: PENDING (test added)

Allgemeine Hinweise
- Fehlerdiagnose ist einfacher, wenn die Konsole-Logs (stderr/stdout) offen sind — prüfe dort auf Pfad-/Asset-Fehler.
- Wenn ein Test fehlschlägt, notiere die Konsolenausgabe und die Schritte; ich kann dann den Fehler gezielt beheben.

Wenn du mir eine erfolgreiche CMake-Build-Ausgabe schickst, markiere ich die erledigten Tests hier im Dokument und fahre mit dem nächsten Roadmap-Punkt fort.
