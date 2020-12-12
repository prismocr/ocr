import matplotlib.pyplot as plt
import sys

def main():
	path = sys.argv[1]
	
	f = open(path, "r")
	lines = f.read().splitlines()
	epochs,batch_size,eta,lmbda = lines[0].split(' ')
	data = [line.split(' ') for line in lines[1:]]
	training_accuracy, evaluation_accuracy = [], []
    
	for d in data:
		training_accuracy.append(float(d[0]))
		evaluation_accuracy.append(float(d[1]))

	plt.title(f"epoch:{epochs} batch_size:{batch_size} eta:{eta} lambda:{lmbda}")

	xPos = range(1,len(data)+1)
	plt.plot(xPos,training_accuracy, label = 'training_accuracy')
	plt.plot(xPos,evaluation_accuracy, label = 'evaluation_accuracy')

	plt.xlabel('epochs')
	plt.ylabel('accuracy %')

	plt.legend()

	minY = min(min(training_accuracy),min(evaluation_accuracy))
	plt.axis([1, int(epochs), minY, 100])

	#plt.show()
	plt.savefig(path[:-3]+'png')
	f.close()

if len(sys.argv) < 2:
    raise Exception("Performance file path missing.")
main()
