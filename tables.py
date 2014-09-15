import re

print '\\documentclass{article}'
print '\\usepackage{fourier}'
print '\\usepackage[margin=0.5in]{geometry}'
print ''
print '\\title{\\textbf{CS4110 -- Assignment 2\\\\Cache Simulator}}'
print '\\author{Viswajith V -- CS11B028\\\\Sriram V -- CS11B058}'
print '\\date{\\today}'
print ''
print '\\begin{document}'
print ''
print '\\maketitle'
print ''
print '\\begin{enumerate}'
for policy in ['LRU', 'LFU', 'RR']:
    print '\\item \\textbf{', policy, '}:\\\\'
    print ''
    print '\\begin{tabular}{ |c|c|c|c|c|c|c|c|c| }'
    print '\\hline'
    print '& \\multicolumn{2}{|c|}{Miss Ratio} & \\multicolumn{2}{|c|}{Cache Hits} & \\multicolumn{2}{|c|}{Memory Accesses} & \\multicolumn{2}{|c|}{Latencies} \\\\  \\hline'
    print 'Dimension & L1 & L2 & L1 & L2 & L1 & L2 & L1 & L2 \\\\  \\hline'
    main_mem_data = []
    for order in [2 **x for x in range(3,11)]:
        temp = [str(order), ' & ']
        filename = 'outputs/cachetrace_'+policy+'_'+str(order)+'.out'
        hits = []
        ratio = []
        accesses = []
        latency = []
        for line in open(filename).readlines():
            if 'hits' in line:
                hits.append(re.findall('\d+', line)[0])
                hits.append(' & ')
            elif 'ratio' in line:
                ratio.append(re.findall('\d+.\d+', line)[0])
                ratio.append(' & ')
            elif 'accesses' in line:
                accesses.append(re.findall('\d+', line)[0])
                accesses.append(' & ')
            elif 'Latency' in line:
                latency.append(re.findall('\d+', line)[0])
                latency.append(' & ')
        temp.extend(accesses[-2:])
        accesses = accesses[:-2]
        temp.append(latency[-2])
        latency = latency[:3]
        latency.append(' \\\\ \\hline')
        temp.append(' \\\\ \\hline')
        main_mem_data.append(temp)
        print order, ' & ', ''.join(ratio), ''.join(hits), ''.join(accesses), ''.join(latency)
    print '\\end{tabular}\\\\\\\\'
    print ''
    print 'For the Main Memory:\\\\'
    print ''
    print '\\begin{tabular}{ |c|c|c| }'
    print '\\hline'
    print 'Dimension & Memory Accesses & Latencies \\\\  \\hline'
    for line in main_mem_data:
        print ''.join(line)
    print '\\end{tabular}\\\\\\\\'
    print ''
print '\\end{enumerate}'
print '\end{document}'
