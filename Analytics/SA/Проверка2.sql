SELECT
   re.real_estate_id,
    re.price,
    (re.price / re.area) AS price_m,
    re.area,
    loc.country,
    loc.region,
    loc.locality,
    loc.street,
    loc.house_number
FROM 
    Real_estates re
JOIN 
    Categories c ON re.category_id = c.category_id
JOIN 
    Locations loc ON re.location_id = loc.location_id
WHERE 
    (re.price / re.area) >= 25000 
    AND (re.price / re.area) <= 210000 
    AND c.flat = TRUE;