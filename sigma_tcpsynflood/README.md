Multi-threaded attack

To ensure we do not need to use locks, which in this case would just slow us down and hinder us from achieving a more efficient attack, we create NB_SOCKETS sockets, and NB_SOCKETS packets that can be modified independently.

We thus achieve maximum efficiency in sending those packets.
