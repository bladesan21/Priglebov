-- Заполнение таблицы Users
INSERT INTO Users (name, surname, phone, email) VALUES
('Иван', 'Иванов', '89991111111', 'ivan@mail.ru'),
('Виктория', 'Викторова', '89992222222', 'viktoria@mail.ru'),
('Василий', 'Васильев', '89993333333', 'vasiliy@mail.ru');

-- Заполнение таблицы Categories
INSERT INTO Categories (house, flat, townhouse, summer_house) VALUES
('TRUE', 'FALSE', 'FALSE', 'FALSE'),
('FALSE', 'TRUE', 'FALSE', 'FALSE'),
('FALSE', 'FALSE', 'TRUE', 'FALSE'),
('FALSE', 'FALSE', 'FALSE', 'TRUE');

-- Заполнение таблицы Locations
INSERT INTO Locations (country, region, locality, street, house_number) VALUES
('РФ', 'Московская область', 'Химки', 'Первая', '1А'),
('РФ', 'Курская область', 'Курск', 'Вторая', '2'),
('РФ', 'NULL', 'Москва', 'Третья', '3');

-- Заполнение таблицы Real_estates
INSERT INTO Real_estates (user_id, price, area, category_id, location_id) VALUES
('1', '5000000', '25', '2', '1'),
('2', '7000000', '70', '1', '2'),
('3', '9000000', '30', '2', '3');