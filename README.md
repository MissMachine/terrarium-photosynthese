# Terrarium – Photosynthèse & données environnementales

## Contexte

Projet STEAM visant à mesurer et interpréter des données
(température, humidité, CO₂, lumière) dans un terrarium.

Les données sont acquises par Arduino, puis extraites via Python 
au format CSV.
Elles sont ensuite traitées et visualisées avec Matplotlib dans des
notebooks JupyterLab.

## Objectifs

- Comprendre la photosynthèse à partir de données réelles (CO2 + luminosité)
- Croiser électronique, sciences et analyse de données
- Proposer un dispositif pédagogique reproductible

## Contenu du dépôt

- `arduino/` : code Arduino
- `notebooks/` : analyse et interprétation (JupyterLab et Marimo)
- `src/` : fonctions Python réutilisables
- `data/` : données brutes et traitées en CSV
- `figures/` : graphiques et schémas

## Matériel

- Arduino : IDE Arduino
- Carte de développement Oneboard AZDelivery +
  capteurs SGP30 (gaz VOCs + eCO2), SHT30-DIS-B (température et humidité), BH1750 (luminosité)
- Carte ESP-32 DEV Kit C V4 AZDelivery
- OLED 0.96" AZDElivery
- Breadboard
- Fils Dupont
- Câbles micro-USB - USB A
- Terrarium
- 3 Calathea makoyana "bébés" (pour la nyctinastie et donc mouvements importants)

## Licence

Voir `LICENSE`
