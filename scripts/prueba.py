# El famoso algoritmo que agrupa las categorias por distrito y
# en base a eso decide la probabilidad de cada crimen.


import csv
import gzip

from collections import Counter, defaultdict

total = Counter()
categories = set()
crimes_by_district = defaultdict(Counter)

with open('train.csv') as f:
    reader = csv.DictReader(f)
    for case in reader:
        crimes_by_district[case['PdDistrict']][case['Category']] += 1
        total[case['PdDistrict']] += 1
        categories.add(case['Category'])

percentage_by_district = defaultdict(Counter)
for district, count in crimes_by_district.items():
    for category, times in count.items():
        percentage = round(times / float(total[district]), 3)
        percentage_by_district[district][category] = percentage

with open('test.csv') as f:
    reader = csv.DictReader(f)
    result = [['Id'] + list(categories)]
    for test in reader:
        row = [test['Id']]
        for category in categories:
            row.append(percentage_by_district[test['PdDistrict']][category])
        result.append(row)

with gzip.open('result.csv.gz', 'wb') as csvfile:
    writer = csv.writer(csvfile, delimiter=',')
    for row in result:
        writer.writerow(row)
