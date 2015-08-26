import Test

print 1

def go( i ):
    j1 = Test.test_function_1(5660 + i, 5454+ i+1)
    j2 = Test.test_function_2(43543 + i, 54534 + i * 2 )
    j3 = Test.test_function_3(435433 + i )
    
    def __cb(b):
        b.foo()
        b.min(1,2)
        
        Test.test_function_5(b)
        pass
    
    j4 = Test.test_function_4(__cb)
    pass
