sudo apt-get install libpq-dev -y
wget http://pqxx.org/download/software/libpqxx/libpqxx-4.0.tar.gz
tar xvfz libpqxx-4.0.tar.gz
cd libpqxx-4.0
./configure
make
sudo make install
sudo apt-get update
sudo apt-get install postgresql postgresql-contrib -y
