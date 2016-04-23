Comment compiler
================
Pour compiler le compilateur de code Metal, utiliser GCC en 32bits.
Sur une x64, utiliser le paramètre -m32, attention a bien avoir gcc+libs en 32 installés sur le système.

Comment compiler le bootcode nominal
====================================
Simplement utiliser le script ./make_source.sh
(Segfault si compilé pour x64)

Liens utiles
============
Compréhension et fonctionnement:
http://nabaztag.forumactif.fr/t13312p30-demande-d-aide-funfoo-firmware-bytecode-du-lapin
http://nabaztag.forumactif.fr/t13241-les-sources-bytecode-et-compilateur

Pour archi x64 (paramètre -m32 déjà intégré dans ce repo)
http://nabaztag.forumactif.fr/t13356-bytecode-compilateur-compiler-sur-un-linux-64bit

Simulateur avec IHM (non testé)
https://github.com/ToToL/NabazTag-Sources

